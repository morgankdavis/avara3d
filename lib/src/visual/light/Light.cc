//
//  Light.cc
//  avara3d
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/visual/light/Light.h"

#include "a3d/Color.h"
#include "a3d/log/Log.h"
#include "a3d/visual/light/AmbientLight.h"
#include "a3d/visual/light/DirectionalLight.h"
#include "a3d/visual/light/PointLight.h"
#include "a3d/visual/light/SpotLight.h"

using namespace std;

namespace a3d {

// [Public Static Member Functions]

shared_ptr<AmbientLight> Light::Ambient() {
    return make_unique<a3d::AmbientLight>();
}

shared_ptr<AmbientLight> Light::Ambient(const Color& color) {
    return make_unique<a3d::AmbientLight>(color);
}

shared_ptr<DirectionalLight> Light::Directional() {
    return make_unique<a3d::DirectionalLight>();
}

shared_ptr<DirectionalLight> Light::Directional(const Color& color) {
    return make_unique<a3d::DirectionalLight>(color);
}

shared_ptr<PointLight> Light::Point() {
    return make_unique<a3d::PointLight>();
}

shared_ptr<PointLight> Light::Point(const Color& color) {
    return make_unique<a3d::PointLight>(color);
}

shared_ptr<SpotLight> Light::Spot() {
    return make_unique<a3d::SpotLight>();
}

shared_ptr<SpotLight> Light::Spot(const Color& color) {
    return make_unique<a3d::SpotLight>(color);
}

// [protected Lifecycle Functions]

Light::Light():
    _name {nullopt},
    _color {Color::White()} {}

Light::Light(const string& name):
    Light {} {

    _name = name;
}

Light::Light(const Color& color):
    Light {} {

    _color = color;
}

Light::Light(const string& name, const Color& color):
    Light {} {

    _name = name;
    _color = color;
}

Light::~Light() {

    if (_name != nullopt) {
        log::d()("Destroying Light '{}' ({:p})", *_name, static_cast<void*>(this));
    }
    else {
        log::d()("Destroying Light {:p}", static_cast<void*>(this));
    }
}

// [Public Member Functions]

const optional<string>& Light::name() const {
    return _name;
}

void Light::name(const string& name) {
    _name = name;
}

const Color& Light::color() const {
    return _color;
}

void Light::color(const Color& color) {
    _color = color;
}

} // namespace a3d
