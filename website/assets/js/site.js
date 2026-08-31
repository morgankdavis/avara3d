(() => {
    "use strict";

    const navToggle = document.querySelector("[data-nav-toggle]");
    const nav = document.querySelector("[data-nav]");

    if (navToggle && nav) {
        navToggle.addEventListener("click", () => {
            const open = navToggle.getAttribute("aria-expanded") === "true";
            navToggle.setAttribute("aria-expanded", String(!open));
            nav.toggleAttribute("data-open", !open);
        });

        nav.addEventListener("click", (event) => {
            if (event.target instanceof HTMLAnchorElement) {
                navToggle.setAttribute("aria-expanded", "false");
                nav.removeAttribute("data-open");
            }
        });
    }

    document.querySelectorAll("[data-current-year]").forEach((element) => {
        element.textContent = String(new Date().getFullYear());
    });

    fetch(resolveSiteUrl("build-info.json"), {cache: "no-store"})
        .then((response) => response.ok ? response.json() : null)
        .then((buildInfo) => {
            if (!buildInfo) {
                return;
            }

            const branch = buildInfo.branch && buildInfo.branch !== "HEAD" ? buildInfo.branch : "detached";
            const commit = typeof buildInfo.commit === "string" ? buildInfo.commit : "";
            const shortCommit = commit.slice(0, 12);
            const summary = `${branch} · ${shortCommit || "unknown"}`;

            document.querySelectorAll("[data-build-summary]").forEach((element) => {
                element.textContent = summary;
            });

            if (commit && commit !== "unknown") {
                document.querySelectorAll("[data-source-repository-link]").forEach((element) => {
                    if (!(element instanceof HTMLAnchorElement)) {
                        return;
                    }

                    element.href =
                        `https://gitlab.mkd.net/a3d/avara3d/-/tree/${encodeURIComponent(commit)}/demos/janus`;
                    element.title = `View ${branch} @ ${shortCommit} in GitLab`;
                });
            }
        })
        .catch(() => {});

    function resolveSiteUrl(path) {
        const inApiDirectory = window.location.pathname.endsWith("/api/") || window.location.pathname.endsWith("/api/index.html");
        return new URL(inApiDirectory ? `../${path}` : path, window.location.href);
    }
})();
