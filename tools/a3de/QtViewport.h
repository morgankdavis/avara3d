//
//  QtViewport.h
//  avara3d
//
//  Created by Morgan Davis on 12/2/2025.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef A3DE_QTVIEWPORT_H
#define A3DE_QTVIEWPORT_H

#include <optional>

#include <QOpenGLWidget>

#include "a3d/render/context/RenderContext.h"

namespace a3d::qt {

class QtInputContext;

class QtViewport : public QOpenGLWidget, public RenderContext {

    Q_OBJECT

signals:
    void initialized();
    void renderFrame();

public:
    // [Public Static Member Functions]

    static std::unique_ptr<QtInputContext> InputContext();

    // [Public Lifecycle Functions]

    explicit QtViewport(Antialiasing antialiasingMode, QWidget* parent = nullptr);
    ~QtViewport() = default;

    // [Public Member Functions]

    bool cursorCaptured() const;
    void cursorCaptured(bool captured);

    // [RenderContext Public Member Functions]

    bool vSyncEnabled() const override;
    void vSyncEnabled(bool enabled) override;

    // [RenderContext Internal Member Functions]

    void beginFrame(const Scene& scene) override;
    void endFrame(const Scene& scene) override;

    void swapBuffers() override;

    math::uvec2 viewportLogicalSize() const override;
    math::uvec2 framebufferSize() const override;

    unsigned defaultFramebuffer() const override;

    // [Internal Member Functions]

    void inputContext(QtInputContext* inputContext);

protected:
    // [QWidget Protected Member Functions]

    bool event(QEvent* e) override;
    void keyPressEvent(QKeyEvent* e) override;
    void keyReleaseEvent(QKeyEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;

    // [QOpenGLWidget Protected Member Functions]

    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

private:
    // [Private Member Functions]

    void centerCursor();

    // [Private Member Variables]

    bool                   _cursorCaptured;
    std::optional<QPointF> _lastCursorPosition;
    std::optional<QPointF> _lastCapturedCursorPosition;
    QtInputContext*        _inputContext;
    bool                   _warpingCursor;
};

} // namespace a3d::qt

#endif // A3DE_QTVIEWPORT_H
