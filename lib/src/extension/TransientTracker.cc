//
//  TransientTracker.cc
//  avara3d
//
//  Created by Morgan Davis on 8/11/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/extension/TransientTracker.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <utility>
#include <vector>

#include "a3d/scene/Node.h"

using namespace a3d;
using namespace a3d::ext;
using namespace std;

// [Public Static Member Functions]

TransientTracker::SweepPolicy TransientTracker::SweepPolicy::EveryUpdate() {
    return {.mode = Mode::EveryUpdate, .updateInterval = 1, .timeInterval = 0.0};
}

TransientTracker::SweepPolicy TransientTracker::SweepPolicy::EveryNUpdates(uint64_t updates) {

    if (updates == 0) {
        throw invalid_argument("TransientNodeRegistry sweep update interval must be at least one.");
    }

    return {.mode = Mode::EveryNUpdates, .updateInterval = updates, .timeInterval = 0.0};
}

TransientTracker::SweepPolicy TransientTracker::SweepPolicy::EveryInterval(double seconds) {

    if (seconds <= 0.0) {
        throw invalid_argument("TransientNodeRegistry sweep time interval must be positive.");
    }

    return {.mode = Mode::EveryInterval, .updateInterval = 1, .timeInterval = seconds};
}

// [Public Lifecycle Functions]

TransientTracker::TransientTracker(SweepPolicy sweepPolicy):
    _entries {},
    _policy {},
    _groupPolicies {},
    _sweepPolicy {sweepPolicy},
    _updatesSinceSweep {0},
    _simulationTime {},
    _nextSweepTime {} {

    validateSweepPolicy(_sweepPolicy);
}

// [Public Member Functions]

void TransientTracker::track(const shared_ptr<Node>& node, const string& group) {

    if (!node) {
        throw invalid_argument("TransientNodeRegistry cannot track a null Node.");
    }

    pruneInactiveEntries();

    if (!node->scene() || node->parent().expired()) {
        throw invalid_argument("TransientNodeRegistry requires a Node attached beneath a Scene root.");
    }

    const auto existing = find_if(_entries.begin(), _entries.end(), [&node](const Entry& entry) {
        if (auto existingNode = entry.node.lock()) {
            return existingNode.get() == node.get();
        }
        return false;
    });

    if (existing != _entries.end()) {
        throw logic_error("TransientNodeRegistry Node is already tracked.");
    }

    _entries.push_back({.node = node, .group = group, .creationTime = _simulationTime.value_or(0.0)});

    enforceCountLimits(group);
}

void TransientTracker::track(const vector<shared_ptr<Node>>& nodes, const string& group) {

    for (const auto& node : nodes) {
        track(node, group);
    }
}

void TransientTracker::untrack(const Node& node) {

    erase_if(_entries, [&node](const Entry& entry) {
        if (auto trackedNode = entry.node.lock()) {
            return trackedNode.get() == &node || !trackedNode->scene() || trackedNode->parent().expired();
        }
        return true;
    });
}

const TransientTracker::Policy& TransientTracker::policy() const {
    return _policy;
}

void TransientTracker::policy(const Policy& policy) {

    validatePolicy(policy);
    _policy = policy;

    pruneInactiveEntries();

    if (_policy.maxCount) {
        enforceMaxCount(*_policy.maxCount, nullptr);
    }
}

void TransientTracker::groupPolicy(const string& group, const Policy& policy) {

    validatePolicy(policy);
    _groupPolicies[group] = policy;

    pruneInactiveEntries();

    if (policy.maxCount) {
        enforceMaxCount(*policy.maxCount, &group);
    }

    if (_policy.maxCount) {
        enforceMaxCount(*_policy.maxCount, nullptr);
    }
}

const TransientTracker::SweepPolicy& TransientTracker::sweepPolicy() const {
    return _sweepPolicy;
}

void TransientTracker::sweepPolicy(const SweepPolicy& policy) {

    validateSweepPolicy(policy);
    _sweepPolicy = policy;
    resetSweepSchedule();
}

void TransientTracker::update(const Scene::StepInfo& info) {

    observeStepInfo(info);
    ++_updatesSinceSweep;

    if (!sweepDue(info)) {
        return;
    }

    performSweep(info.endTime);
    _updatesSinceSweep = 0;

    if (_sweepPolicy.mode == SweepPolicy::Mode::EveryInterval && _nextSweepTime) {
        const double elapsedIntervals =
            floor((info.endTime - *_nextSweepTime) / _sweepPolicy.timeInterval) + 1.0;
        *_nextSweepTime += elapsedIntervals * _sweepPolicy.timeInterval;

        if (*_nextSweepTime <= info.endTime) {
            *_nextSweepTime = info.endTime + _sweepPolicy.timeInterval;
        }
    }
}

void TransientTracker::sweep(const Scene::StepInfo& info) {

    observeStepInfo(info);
    performSweep(info.endTime);
    resetSweepSchedule();
}

void TransientTracker::clear() {

    _entries.clear();
    resetRuntimeState();
}

void TransientTracker::removeAll() {

    vector<shared_ptr<Node>> nodes;
    nodes.reserve(_entries.size());

    for (const auto& entry : _entries) {
        if (auto node = entry.node.lock(); node && !node->parent().expired()) {
            nodes.push_back(std::move(node));
        }
    }

    _entries.clear();
    resetRuntimeState();
    detachNodes(nodes);
}

// [Private Static Member Functions]

void TransientTracker::validatePolicy(const Policy& policy) {

    if (policy.maxAge && (*policy.maxAge < 0.0)) {
        throw invalid_argument("TransientNodeRegistry maximum age must be non-negative.");
    }

    if (policy.distanceLimit) {
        if (policy.distanceLimit->radius < 0.0f) {
            throw invalid_argument("TransientNodeRegistry distance-limit radius must be non-negative.");
        }
    }
}

void TransientTracker::validateSweepPolicy(const SweepPolicy& policy) {

    switch (policy.mode) {

        case SweepPolicy::Mode::EveryUpdate:
            break;

        case SweepPolicy::Mode::EveryNUpdates:
            if (policy.updateInterval == 0) {
                throw invalid_argument("TransientNodeRegistry sweep update interval must be at least one.");
            }
            break;

        case SweepPolicy::Mode::EveryInterval:
            if (policy.timeInterval <= 0.0) {
                throw invalid_argument("TransientNodeRegistry sweep time interval must be positive.");
            }
            break;

        default:
            throw invalid_argument("TransientNodeRegistry sweep policy has an invalid mode.");
    }
}

void TransientTracker::detachNodes(const vector<shared_ptr<Node>>& nodes) {

    for (const auto& node : nodes) {
        if (node && !node->parent().expired()) {
            node->removeFromParent();
        }
    }
}

// [Private Member Functions]

void TransientTracker::observeStepInfo(const Scene::StepInfo& info) {

    const bool newTimeline = !_simulationTime || info.endTime < *_simulationTime;
    if (newTimeline) {
        for (auto& entry : _entries) {
            entry.creationTime = info.startTime;
        }

        _updatesSinceSweep = 0;
        _nextSweepTime.reset();
    }

    _simulationTime = info.endTime;

    if (_sweepPolicy.mode == SweepPolicy::Mode::EveryInterval && !_nextSweepTime) {
        _nextSweepTime = info.startTime + _sweepPolicy.timeInterval;
    }
}

bool TransientTracker::sweepDue(const Scene::StepInfo& info) const {

    switch (_sweepPolicy.mode) {

        case SweepPolicy::Mode::EveryUpdate:
            return true;

        case SweepPolicy::Mode::EveryNUpdates:
            return _updatesSinceSweep >= _sweepPolicy.updateInterval;

        case SweepPolicy::Mode::EveryInterval:
            return _nextSweepTime && info.endTime >= *_nextSweepTime;
    }

    return false;
}

void TransientTracker::performSweep(double simulationTime) {

    vector<shared_ptr<Node>> nodes;

    erase_if(_entries, [this, simulationTime, &nodes](const Entry& entry) {
        auto node = entry.node.lock();
        if (!node || !node->scene() || node->parent().expired()) {
            return true;
        }

        if (!shouldRemove(entry, *node, simulationTime)) {
            return false;
        }

        nodes.push_back(std::move(node));
        return true;
    });

    detachNodes(nodes);
}

bool TransientTracker::shouldRemove(const Entry& entry, const Node& node, double simulationTime) const {

    if (policyRemoves(_policy, entry, node, simulationTime)) {
        return true;
    }

    const auto groupPolicy = _groupPolicies.find(entry.group);
    return groupPolicy != _groupPolicies.end()
           && policyRemoves(groupPolicy->second, entry, node, simulationTime);
}

bool TransientTracker::policyRemoves(const Policy& policy,
                               const Entry&  entry,
                               const Node&   node,
                               double        simulationTime) const {

    if (policy.maxAge) {
        const double age = simulationTime >= entry.creationTime ? simulationTime - entry.creationTime : 0.0;
        if (age >= *policy.maxAge) {
            return true;
        }
    }

    if (policy.distanceLimit) {
        const auto   position = node.worldPosition();
        const double x = static_cast<double>(position.x) - policy.distanceLimit->center.x;
        const double y = static_cast<double>(position.y) - policy.distanceLimit->center.y;
        const double z = static_cast<double>(position.z) - policy.distanceLimit->center.z;
        const double radius = policy.distanceLimit->radius;

        if (x * x + y * y + z * z > radius * radius) {
            return true;
        }
    }

    return false;
}

void TransientTracker::pruneInactiveEntries() {

    erase_if(_entries, [](const Entry& entry) {
        if (auto node = entry.node.lock()) {
            return !node->scene() || node->parent().expired();
        }
        return true;
    });
}

void TransientTracker::enforceCountLimits(const string& group) {

    const auto groupPolicy = _groupPolicies.find(group);
    if (groupPolicy != _groupPolicies.end() && groupPolicy->second.maxCount) {
        enforceMaxCount(*groupPolicy->second.maxCount, &group);
    }

    if (_policy.maxCount) {
        enforceMaxCount(*_policy.maxCount, nullptr);
    }
}

void TransientTracker::enforceMaxCount(size_t maxCount, const string* group) {

    const auto matches = [group](const Entry& entry) {
        return !group || entry.group == *group;
    };

    const size_t count = static_cast<size_t>(count_if(_entries.begin(), _entries.end(), matches));
    if (count <= maxCount) {
        return;
    }

    size_t                   remaining = count - maxCount;
    vector<shared_ptr<Node>> nodes;
    nodes.reserve(remaining);

    erase_if(_entries, [&matches, &remaining, &nodes](const Entry& entry) {
        if (remaining == 0 || !matches(entry)) {
            return false;
        }

        if (auto node = entry.node.lock(); node && node->scene() && !node->parent().expired()) {
            nodes.push_back(std::move(node));
        }

        --remaining;
        return true;
    });

    detachNodes(nodes);
}

void TransientTracker::resetSweepSchedule() {

    _updatesSinceSweep = 0;
    _nextSweepTime.reset();

    if (_sweepPolicy.mode == SweepPolicy::Mode::EveryInterval && _simulationTime) {
        _nextSweepTime = *_simulationTime + _sweepPolicy.timeInterval;
    }
}

void TransientTracker::resetRuntimeState() {

    _updatesSinceSweep = 0;
    _simulationTime.reset();
    _nextSweepTime.reset();
}
