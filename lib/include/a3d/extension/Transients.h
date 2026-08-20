//
//  Transients.h
//  avara3d
//
//  Created by Morgan Davis on 8/11/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_EXTENSION_TRANSIENTS_H
#define AVARA3D_EXTENSION_TRANSIENTS_H

#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "a3d/Math.h"
#include "a3d/scene/Scene.h"

namespace a3d {

    class Node;

}

namespace a3d::ext {

    /**
     * @brief Tracks transient scene nodes and removes them according to configurable policies.
     *
     * The registry stores weak references and never owns tracked nodes. Count limits are
     * enforced immediately when nodes are tracked or policies change. Age and distance
     * limits are evaluated only when a configured sweep is due or sweep() is called
     * explicitly.
     *
     * Functions that may remove nodes must be called only from a point where scene mutation
     * is safe, such as after a physics simulation step has completed.
     */
    class Transients {

    public:
        // [Public Types]

        /** @brief Maximum permitted world-space distance from a fixed point. */
        struct DistanceLimit {

            /** World-space center used for the distance test. */
            math::vec3 center {0.0f};

            /** Maximum permitted distance from center. */
            float radius {0.0f};
        };

        /** @brief Removal limits applied globally or to a named group. */
        struct Policy {

            /**
             * Maximum number of matching tracked nodes.
             *
             * Excess nodes are removed immediately, oldest first.
             */
            std::optional<std::size_t> maxCount {};

            /**
             * Maximum node age in seconds of simulation time.
             *
             * The limit is evaluated during sweeps.
             */
            std::optional<double> maxAge {};

            /**
             * Maximum world-space distance from a fixed point.
             *
             * The limit is evaluated during sweeps.
             */
            std::optional<DistanceLimit> distanceLimit {};
        };

        /** @brief Scheduling policy controlling when update() performs a sweep. */
        struct SweepPolicy {

            /** @brief Available sweep scheduling modes. */
            enum class Mode {
                EveryUpdate,   ///< Sweep on every call to update().
                EveryNUpdates, ///< Sweep after a configured number of calls to update().
                EveryInterval  ///< Sweep after a configured interval of simulation time.
            };

            /** Selected scheduling mode. */
            Mode mode {Mode::EveryUpdate};

            /** Number of update() calls between sweeps in Mode::EveryNUpdates. */
            std::uint64_t updateInterval {1};

            /** Seconds of simulation time between sweeps in Mode::EveryInterval. */
            double timeInterval {0.0};

            /**
             * @brief Creates a policy that sweeps on every call to update().
             *
             * @return the configured sweep policy.
             */
            static SweepPolicy EveryUpdate();

            /**
             * @brief Creates a policy that sweeps after a fixed number of update() calls.
             *
             * @param updates Number of update() calls between sweeps.
             * @return the configured sweep policy.
             *
             * @throws std::invalid_argument if updates is zero.
             */
            static SweepPolicy EveryNUpdates(std::uint64_t updates);

            /**
             * @brief Creates a policy that sweeps at a fixed simulation-time interval.
             *
             * @param seconds Seconds of simulation time between sweeps.
             * @return the configured sweep policy.
             *
             * @throws std::invalid_argument if seconds is non-finite or not positive.
             */
            static SweepPolicy EveryInterval(double seconds);
        };

        // [Public Lifecycle Functions]

        /**
         * @brief Creates an empty registry with the supplied sweep policy.
         *
         * @param sweepPolicy Scheduling policy used by update().
         *
         * @throws std::invalid_argument if sweepPolicy is invalid.
         */
        explicit Transients(SweepPolicy sweepPolicy = SweepPolicy::EveryUpdate());

        Transients(const Transients&)            = delete;
        Transients& operator=(const Transients&) = delete;

        Transients(Transients&&)            = delete;
        Transients& operator=(Transients&&) = delete;

        /** @brief Destroys the registry without removing tracked nodes. */
        ~Transients() = default;

        // [Public Member Functions]

        /**
         * @brief Tracks a transient node and immediately enforces applicable count limits.
         *
         * The node's creation time is the most recently observed simulation time. Before the first update() or
         * sweep(), tracked nodes are assigned the start time of the first supplied Scene::StepInfo.
         *
         * @param node Node to track. The registry retains only a weak reference.
         * @param group Optional group used to select an additional group policy.
         *
         * @throws std::invalid_argument if node is null or is not attached beneath a Scene root.
         * @throws std::logic_error if node is already tracked.
         */
        void track(const std::shared_ptr<Node>& node, const std::string& group = {});

        /**
         * @brief Tracks a collection of nodes as members of the same transient group.
         *
         * Each node is registered using the same behavior as the single-node track()
         * overload. Group and global count limits are enforced as nodes are added.
         *
         * @param nodes Nodes to track.
         * @param group Optional transient group shared by all nodes.
         */
        void track(const std::vector<std::shared_ptr<Node>>& nodes, const std::string& group = {});

        /**
         * @brief Stops tracking a node without removing it from its parent.
         *
         * @param node Node to stop tracking.
         */
        void untrack(const Node& node);

        /** @brief Returns the global removal policy. */
        const Policy& policy() const;

        /**
         * @brief Replaces the global removal policy.
         *
         * A reduced maxCount is enforced immediately. Age and distance limits take effect on the next sweep.
         *
         * @param policy New global policy.
         *
         * @throws std::invalid_argument if policy contains an invalid age or distance limit.
         */
        void policy(const Policy& policy);

        /**
         * @brief Replaces the removal policy for a named group.
         *
         * A reduced maxCount is enforced immediately. Age and distance limits take effect on the next sweep. An
         * empty group name configures the policy applied to nodes tracked without a group name.
         *
         * @param group Group whose policy is replaced.
         * @param policy New group policy.
         *
         * @throws std::invalid_argument if policy contains an invalid age or distance limit.
         */
        void groupPolicy(const std::string& group, const Policy& policy);

        /** @brief Returns the scheduling policy used by update(). */
        const SweepPolicy& sweepPolicy() const;

        /**
         * @brief Replaces the scheduling policy used by update().
         *
         * The current scheduling cadence is restarted when the policy changes.
         *
         * @param policy New sweep policy.
         *
         * @throws std::invalid_argument if policy is invalid.
         */
        void sweepPolicy(const SweepPolicy& policy);

        /**
         * @brief Advances registry scheduling and sweeps when the configured policy is due.
         *
         * One call to this function counts as one update for SweepPolicy::EveryNUpdates.
         * Time-based scheduling uses Scene::StepInfo::endTime and therefore advances in
         * simulation time rather than wall-clock time.
         *
         * @param info Information for the completed simulation step.
         *
         * @throws std::invalid_argument if info contains invalid simulation times.
         */
        void update(const Scene::StepInfo& info);

        /**
         * @brief Immediately evaluates all swept removal policies.
         *
         * Calling this function restarts the configured update-count or simulation-time
         * cadence from this sweep.
         *
         * @param info Information for the completed simulation step.
         *
         * @throws std::invalid_argument if info contains invalid simulation times.
         */
        void sweep(const Scene::StepInfo& info);

        /**
         * @brief Forgets every tracked node and resets runtime scheduling state.
         *
         * Tracked nodes are not removed from their parents. Global, group, and sweep policies are retained.
         */
        void clear();

        /**
         * @brief Removes every live tracked node from its parent and clears the registry.
         *
         * Global, group, and sweep policies are retained.
         */
        void removeAll();

    private:
        // [Private Types]

        struct Entry {
            std::weak_ptr<Node> node;
            std::string         group;
            double              creationTime {0.0};
        };

        // [Private Static Member Functions]

        static void        validatePolicy(const Policy& policy);
        static void        validateSweepPolicy(const SweepPolicy& policy);
        static void        validateStepInfo(const Scene::StepInfo& info);
        static bool        isFinite(const math::vec3& value);
        static void        detachNodes(const std::vector<std::shared_ptr<Node>>& nodes);

        // [Private Member Functions]

        void               observeStepInfo(const Scene::StepInfo& info);
        bool               sweepDue(const Scene::StepInfo& info) const;
        void               performSweep(double simulationTime);

        bool               shouldRemove(const Entry& entry, const Node& node, double simulationTime) const;
        bool               policyRemoves(const Policy& policy,
                                         const Entry& entry,
                                         const Node& node,
                                         double simulationTime) const;

        void               pruneInactiveEntries();
        void               enforceCountLimits(const std::string& group);
        void               enforceMaxCount(std::size_t maxCount, const std::string* group);

        void               resetSweepSchedule();
        void               resetRuntimeState();

        // [Private Member Variables]

        std::vector<Entry>                      _entries;
        Policy                                  _policy;
        std::unordered_map<std::string, Policy> _groupPolicies;
        SweepPolicy                             _sweepPolicy;
        std::uint64_t                           _updatesSinceSweep;
        std::optional<double>                   _simulationTime;
        std::optional<double>                   _nextSweepTime;
    };

}

#endif //AVARA3D_EXTENSION_TRANSIENTS_H
