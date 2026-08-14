(() => {
    "use strict";

    const stage = document.querySelector("[data-demo-stage]");
    const canvas = document.getElementById("canvas");
    const status = document.getElementById("demo-status");
    const statusText = document.getElementById("demo-status-text");

    if (!stage || !(canvas instanceof HTMLCanvasElement) || !status || !statusText) {
        return;
    }

    const scriptPath = stage.dataset.demoScript || "demo/janus.js";
    let runtimeReady = false;

    canvas.addEventListener("click", () => canvas.focus());

    const setStatus = (message) => {
        if (!message) {
            runtimeReady = true;
            stage.dataset.ready = "true";
            status.setAttribute("hidden", "");
            return;
        }

        status.removeAttribute("hidden");
        statusText.textContent = message.replace(/\s*\(\d+(?:\.\d+)?\/\d+\)$/, "");
    };

    const showFailure = () => {
        stage.dataset.failed = "true";
        status.classList.add("demo-status-error");
        statusText.innerHTML = "Web build not found. Build and install <code>janus</code>, then refresh.";
    };

    window.Module = {
        canvas,
        noExitRuntime: true,
        locateFile(path) {
            return new URL(`demo/${path}`, window.location.href).href;
        },
        print(text) {
            console.log(`[Avara3D] ${text}`);
        },
        printErr(text) {
            console.error(`[Avara3D] ${text}`);
        },
        setStatus,
        monitorRunDependencies(remaining) {
            if (runtimeReady) {
                return;
            }
            setStatus(remaining > 0 ? `Preparing demo (${remaining})` : "Starting simulation…");
        },
        onRuntimeInitialized() {
            setStatus("");
            canvas.focus({preventScroll: true});
        }
    };

    const script = document.createElement("script");
    script.src = scriptPath;
    script.async = true;
    script.addEventListener("error", showFailure, {once: true});
    document.body.append(script);

    window.setTimeout(() => {
        if (!runtimeReady && !stage.dataset.failed) {
            statusText.textContent = "Still loading the simulation…";
        }
    }, 10000);
})();
