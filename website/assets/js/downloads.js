(() => {
    "use strict";

    const r2DownloadsBaseByHost = {
        "staging.avara3d.net":
            "https://cdn.avara3d.net/staging/downloads/",
        "avara3d.net":
            "https://cdn.avara3d.net/production/downloads/"
    };

    const downloadsBase =
        r2DownloadsBaseByHost[window.location.hostname] ||
        new URL("downloads/", window.location.href).href;

    document
        .querySelectorAll("[data-download-checksum]")
        .forEach((link) => {
            const channel = link.dataset.downloadChecksum;

            link.href = new URL(
                `${channel}/SHA256SUMS`,
                downloadsBase
            ).href;
        });

    document
        .querySelectorAll("[data-download-channel]")
        .forEach((panel) => {
            void loadDownloadChannel(panel);
        });

    async function loadDownloadChannel(panel) {
        const channel = panel.dataset.downloadChannel;
        const summary = panel.querySelector("[data-download-summary]");

        if (!channel || !summary) {
            return;
        }

        const manifestUrl = new URL(
            `${channel}/manifest.json`,
            downloadsBase
        );

        try {
            const response = await fetch(manifestUrl, {
                cache: "no-store"
            });

            if (!response.ok) {
                throw new Error(
                    `Manifest request failed with status ${response.status}`
                );
            }

            const manifest = await response.json();

            if (
                manifest.channel !== channel ||
                !manifest.packages ||
                typeof manifest.packages !== "object"
            ) {
                throw new Error("Invalid downloads manifest");
            }

            summary.textContent = formatSummary(manifest);

            panel
                .querySelectorAll("[data-download-platform]")
                .forEach((row) => {
                    const platform = row.dataset.downloadPlatform;
                    const link = row.querySelector("[data-download-link]");
                    const packageInfo = manifest.packages[platform];

                    if (!link) {
                        return;
                    }

                    if (!isValidPackage(packageInfo)) {
                        markUnavailable(link);
                        return;
                    }

                    const packageUrl = new URL(
                        packageInfo.filename,
                        manifestUrl
                    );

                    link.href = packageUrl.href;
                    link.download = packageInfo.filename;
                    link.textContent =
                        `Download · ${formatBytes(packageInfo.size)}`;
                    link.title = `SHA-256: ${packageInfo.sha256}`;
                    link.removeAttribute("aria-disabled");
                });
        } catch (error) {
            summary.textContent = "Not published yet";

            panel
                .querySelectorAll("[data-download-link]")
                .forEach(markUnavailable);

            console.warn(
                `Could not load ${channel} downloads manifest:`,
                error
            );
        }
    }

    function isValidPackage(packageInfo) {
        return (
            packageInfo &&
            typeof packageInfo.filename === "string" &&
            /^[A-Za-z0-9._-]+$/.test(packageInfo.filename) &&
            typeof packageInfo.sha256 === "string"
        );
    }

    function markUnavailable(link) {
        link.removeAttribute("href");
        link.removeAttribute("download");
        link.removeAttribute("title");
        link.setAttribute("aria-disabled", "true");
        link.textContent = "Not available";
    }

    function formatSummary(manifest) {
        const parts = [];

        if (
            typeof manifest.version === "string" &&
            /^\d+\.\d+\.\d+$/.test(manifest.version)
        ) {
            parts.push(`v${manifest.version}`);
        }

        const buildNumber = Number.isInteger(manifest.buildNumber)
            ? manifest.buildNumber
            : Number.isInteger(manifest.pipelineIid)
                ? manifest.pipelineIid
                : null;

        if (buildNumber !== null) {
            parts.push(`build ${buildNumber}`);
        }

        if (
            typeof manifest.shortCommit === "string" &&
            manifest.shortCommit
        ) {
            parts.push(manifest.shortCommit);
        }

        return parts.length > 0
            ? parts.join(" · ")
            : manifest.publicLabel || manifest.channel;
    }

    function formatBytes(byteCount) {
        if (!Number.isFinite(byteCount) || byteCount < 0) {
            return "package";
        }

        if (byteCount < 1024) {
            return `${byteCount} B`;
        }

        const units = ["KB", "MB", "GB"];
        let value = byteCount / 1024;
        let unit = units[0];

        for (let index = 1;
             index < units.length && value >= 1024;
             ++index) {

            value /= 1024;
            unit = units[index];
        }

        const precision = value >= 100 ? 0 : 1;
        return `${value.toFixed(precision)} ${unit}`;
    }
})();
