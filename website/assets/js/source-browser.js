(() => {
    "use strict";

    const browser = document.querySelector("[data-source-browser]");

    if (!browser) {
        return;
    }

    const codeView = browser.querySelector("[data-source-code]");
    const title = browser.querySelector("[data-source-title]");
    const pathLabel = browser.querySelector("[data-source-path]");
    const buttons = Array.from(browser.querySelectorAll("[data-source-file]"));

    if (!codeView || !title || !pathLabel || buttons.length === 0) {
        return;
    }

    const files = new Set(buttons.map((button) => button.dataset.sourceFile));
    const requestedFile = new URLSearchParams(window.location.search).get("file");
    const initialFile = requestedFile && files.has(requestedFile) ? requestedFile : "main.cc";

    buttons.forEach((button) => {
        button.addEventListener("click", () => loadFile(button.dataset.sourceFile));
    });

    loadFile(initialFile);

    async function loadFile(fileName) {
        if (!fileName || !files.has(fileName)) {
            return;
        }

        buttons.forEach((button) => {
            const selected = button.dataset.sourceFile === fileName;
            button.toggleAttribute("data-selected", selected);
            button.setAttribute("aria-pressed", String(selected));
        });

        title.textContent = fileName;
        pathLabel.textContent = `demos/janus/${fileName}`;
        codeView.innerHTML = '<div class="source-loading">Loading source…</div>';

        const url = new URL(`source/${fileName}`, window.location.href);

        try {
            const response = await fetch(url, {cache: "no-store"});

            if (!response.ok) {
                throw new Error(`HTTP ${response.status}`);
            }

            const source = await response.text();
            renderSource(source);

            const currentUrl = new URL(window.location.href);
            currentUrl.searchParams.set("file", fileName);
            window.history.replaceState({}, "", currentUrl);
        }
        catch (error) {
            codeView.innerHTML = '<div class="source-error">Source file unavailable. Run <code>tools/website/serve.sh</code> from the repository root or assemble the website bundle.</div>';
            console.error(`Unable to load ${fileName}:`, error);
        }
    }

    function renderSource(source) {
        const highlightedLines = highlightCpp(source.replace(/\r\n/g, "\n"));
        const fragment = document.createDocumentFragment();

        highlightedLines.forEach((line, index) => {
            const row = document.createElement("div");
            row.className = "code-line";
            row.innerHTML = `<span class="line-number" aria-hidden="true">${index + 1}</span><code>${line || " "}</code>`;
            fragment.append(row);
        });

        codeView.replaceChildren(fragment);
        codeView.scrollTop = 0;
        codeView.scrollLeft = 0;
    }

    function highlightCpp(source) {
        const prism = window.Prism;

        if (!prism || !prism.languages || !prism.languages.cpp) {
            return source.split("\n").map(escapeHtml);
        }

        const lines = [""];
        const highlighted = prism.tokenize(source, prism.languages.cpp);

        appendHighlighted(highlighted, []);
        return lines;

        function appendHighlighted(value, classes) {
            if (typeof value === "string") {
                appendText(value, classes);
                return;
            }

            if (Array.isArray(value)) {
                value.forEach((part) => appendHighlighted(part, classes));
                return;
            }

            if (!value || typeof value !== "object") {
                return;
            }

            const aliases = Array.isArray(value.alias)
                ? value.alias
                : value.alias
                    ? [value.alias]
                    : [];

            appendHighlighted(value.content, [...classes, "token", value.type, ...aliases]);
        }

        function appendText(value, classes) {
            value.split("\n").forEach((part, index) => {
                if (index > 0) {
                    lines.push("");
                }

                if (part) {
                    lines[lines.length - 1] += wrapToken(escapeHtml(part), classes);
                }
            });
        }

        function wrapToken(value, classes) {
            const className = [...new Set(classes)]
                .filter((name) => /^[A-Za-z0-9_-]+$/.test(name))
                .join(" ");

            return className
                ? `<span class="${className}">${value}</span>`
                : value;
        }
    }

    function escapeHtml(value) {
        return value
            .replaceAll("&", "&amp;")
            .replaceAll("<", "&lt;")
            .replaceAll(">", "&gt;")
            .replaceAll('"', "&quot;");
    }
})();
