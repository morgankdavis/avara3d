(() => {
    "use strict";

    const browser = document.querySelector("[data-source-browser]");

    if (!browser) {
        return;
    }

    const contentView = browser.querySelector("[data-source-content]");
    const directoryLabel = browser.querySelector("[data-source-directory]");
    const directoryList = browser.querySelector("[data-source-directory-list]");
    const languageLabel = browser.querySelector("[data-source-language]");
    const title = browser.querySelector("[data-source-title]");
    const pathLabel = browser.querySelector("[data-source-path]");

    if (!contentView || !directoryLabel || !directoryList || !languageLabel || !title || !pathLabel) {
        return;
    }

    const rootDescriptions = new Map([
        ["main.cc", "Application entry point"],
        ["App.h", "Application interface"],
        ["App.cc", "Scene and fixed-step behavior"],
    ]);
    const rootFileOrder = new Map([
        ["main.cc", 0],
        ["App.h", 1],
        ["App.cc", 2],
    ]);

    let currentDirectory = "";
    let demoName = "janus";
    let fileLoadSequence = 0;
    let files = new Map();
    let selectedFile = "";

    initialize();

    async function initialize() {
        try {
            const manifestUrl = new URL("source-manifest.json", window.location.href);
            const response = await fetch(manifestUrl, {cache: "no-store"});

            if (!response.ok) {
                throw new Error(`HTTP ${response.status}`);
            }

            const manifest = await response.json();
            demoName = typeof manifest.demo === "string" && manifest.demo ? manifest.demo : demoName;
            files = parseFiles(manifest.files);

            if (files.size === 0) {
                throw new Error("The source manifest does not contain any files.");
            }

            const search = new URLSearchParams(window.location.search);
            const requestedFile = normalizePath(search.get("file"));
            const requestedDirectory = normalizePath(search.get("path"));
            const validRequestedFile = requestedFile && files.has(requestedFile) ? requestedFile : "";

            currentDirectory = directoryExists(requestedDirectory)
                ? requestedDirectory
                : validRequestedFile
                    ? parentDirectory(validRequestedFile)
                    : "";

            renderDirectory();

            const initialFile = validRequestedFile || (files.has("main.cc") ? "main.cc" : files.keys().next().value);
            loadFile(initialFile, false);
        }
        catch (error) {
            directoryList.innerHTML = '<div class="source-list-error">File list unavailable.</div>';
            renderError("Demo files unavailable. Run tools/website/serve.sh from the repository root or assemble the website bundle.");
            console.error("Unable to initialize the demo source browser:", error);
        }
    }

    function parseFiles(entries) {
        const parsed = new Map();

        if (!Array.isArray(entries)) {
            return parsed;
        }

        entries.forEach((entry) => {
            if (!entry || typeof entry.path !== "string") {
                return;
            }

            const path = normalizePath(entry.path);

            if (!path || path !== entry.path || path.split("/").some((part) => part === "..")) {
                return;
            }

            parsed.set(path, {
                path,
                size: Number.isFinite(entry.size) && entry.size >= 0 ? entry.size : null,
            });
        });

        return parsed;
    }

    function renderDirectory() {
        const fragment = document.createDocumentFragment();
        const children = directoryChildren(currentDirectory);
        const fullDirectoryName = currentDirectory ? `${demoName}/${currentDirectory}` : demoName;

        directoryLabel.textContent = fullDirectoryName;
        directoryLabel.title = fullDirectoryName;

        if (currentDirectory) {
            const parent = parentDirectory(currentDirectory);
            const parentName = parent ? baseName(parent) : demoName;
            fragment.append(createBackButton(parent, parentName));
        }

        children.forEach((entry) => {
            fragment.append(entry.directory ? createDirectoryButton(entry) : createFileButton(entry));
        });

        if (children.length === 0) {
            const empty = document.createElement("div");
            empty.className = "source-list-empty";
            empty.textContent = "This directory is empty.";
            fragment.append(empty);
        }

        directoryList.replaceChildren(fragment);
    }

    function directoryChildren(directory) {
        const prefix = directory ? `${directory}/` : "";
        const directories = new Map();
        const directFiles = [];

        files.forEach((file) => {
            if (!file.path.startsWith(prefix)) {
                return;
            }

            const relativePath = file.path.slice(prefix.length);
            const slash = relativePath.indexOf("/");

            if (slash === -1) {
                directFiles.push({
                    ...file,
                    directory: false,
                    name: relativePath,
                });
                return;
            }

            const name = relativePath.slice(0, slash);
            const path = prefix + name;
            const existing = directories.get(path);

            if (existing) {
                existing.fileCount += 1;
            }
            else {
                directories.set(path, {
                    directory: true,
                    fileCount: 1,
                    name,
                    path,
                });
            }
        });

        const directoryEntries = [...directories.values()].sort(compareNames);
        directFiles.sort((left, right) => {
            if (!directory) {
                const leftOrder = rootFileOrder.get(left.path) ?? Number.MAX_SAFE_INTEGER;
                const rightOrder = rootFileOrder.get(right.path) ?? Number.MAX_SAFE_INTEGER;

                if (leftOrder !== rightOrder) {
                    return leftOrder - rightOrder;
                }
            }

            return compareNames(left, right);
        });

        return directory ? [...directoryEntries, ...directFiles] : [...directFiles, ...directoryEntries];
    }

    function createBackButton(parent, parentName) {
        const button = createRowButton(`Back to ${parentName}`, `‹ ${parentName}`, "Parent directory");
        button.classList.add("source-directory-back");
        button.addEventListener("click", () => navigateToDirectory(parent));
        return button;
    }

    function createDirectoryButton(entry) {
        const description = entry.path === "data"
            ? "Runtime assets"
            : `${entry.fileCount} ${entry.fileCount === 1 ? "file" : "files"}`;
        const button = createRowButton(`Open ${entry.name}`, entry.name, description, "›");
        button.classList.add("source-directory-row");
        button.addEventListener("click", () => navigateToDirectory(entry.path));
        return button;
    }

    function createFileButton(entry) {
        const button = createRowButton(
            `Preview ${entry.name}`,
            entry.name,
            rootDescriptions.get(entry.path) || fileDescription(entry),
        );
        button.classList.add("source-file-row");
        button.toggleAttribute("data-selected", entry.path === selectedFile);
        button.setAttribute("aria-pressed", String(entry.path === selectedFile));
        button.addEventListener("click", () => loadFile(entry.path));
        return button;
    }

    function createRowButton(label, name, description, chevron = "") {
        const button = document.createElement("button");
        const copy = document.createElement("span");
        const strong = document.createElement("strong");
        const small = document.createElement("small");

        button.type = "button";
        button.className = "source-browser-row";
        button.setAttribute("aria-label", label);
        copy.className = "source-browser-row-copy";
        strong.textContent = name;
        small.textContent = description;
        copy.append(strong, small);
        button.append(copy);

        if (chevron) {
            const indicator = document.createElement("span");
            indicator.className = "source-row-chevron";
            indicator.setAttribute("aria-hidden", "true");
            indicator.textContent = chevron;
            button.append(indicator);
        }

        return button;
    }

    function navigateToDirectory(path) {
        if (!directoryExists(path)) {
            return;
        }

        currentDirectory = path;
        renderDirectory();
        updateUrl();
        directoryList.scrollTop = 0;
    }

    async function loadFile(path, updateHistory = true) {
        const file = files.get(path);

        if (!file) {
            return;
        }

        selectedFile = path;
        renderDirectory();
        const preview = previewType(path);
        const sequence = ++fileLoadSequence;
        const url = sourceUrl(path);

        languageLabel.textContent = preview.label;
        title.textContent = baseName(path);
        pathLabel.textContent = `demos/${demoName}/${path}`;
        setLoading();

        if (updateHistory) {
            updateUrl();
        }

        if (preview.kind === "image") {
            renderImage(url, path, sequence);
            return;
        }

        if (preview.kind === "binary") {
            renderBinary(file, preview.label);
            return;
        }

        if (preview.kind === "file") {
            renderBinary(file, preview.label);
            return;
        }

        try {
            const response = await fetch(url, {cache: "no-store"});

            if (!response.ok) {
                throw new Error(`HTTP ${response.status}`);
            }

            const source = await response.text();

            if (sequence !== fileLoadSequence) {
                return;
            }

            renderSource(source, preview.language);
        }
        catch (error) {
            if (sequence === fileLoadSequence) {
                renderError("This file could not be loaded.");
            }
            console.error(`Unable to load ${path}:`, error);
        }
    }

    function renderSource(source, language) {
        contentView.dataset.previewKind = "source";
        contentView.setAttribute("aria-label", `${language === "json" ? "JSON" : "C++"} source`);
        const highlightedLines = highlightSource(source.replace(/\r\n/g, "\n"), language);
        const fragment = document.createDocumentFragment();

        highlightedLines.forEach((line, index) => {
            const row = document.createElement("div");
            row.className = "code-line";
            row.innerHTML = `<span class="line-number" aria-hidden="true">${index + 1}</span><code>${line || " "}</code>`;
            fragment.append(row);
        });

        contentView.replaceChildren(fragment);
        resetContentScroll();
    }

    function renderImage(url, path, sequence) {
        const preview = document.createElement("div");
        const image = document.createElement("img");

        contentView.dataset.previewKind = "image";
        contentView.setAttribute("aria-label", `Image preview of ${baseName(path)}`);
        preview.className = "source-image-preview";
        image.alt = `Preview of ${baseName(path)}`;
        image.addEventListener("load", () => {
            if (sequence !== fileLoadSequence) {
                return;
            }
            contentView.replaceChildren(preview);
            resetContentScroll();
        });
        image.addEventListener("error", () => {
            if (sequence === fileLoadSequence) {
                renderError("This image could not be loaded.");
            }
        });
        preview.append(image);
        image.src = url;
    }

    function renderBinary(file, label) {
        const preview = document.createElement("div");
        const icon = document.createElement("div");
        const name = document.createElement("strong");
        const details = document.createElement("span");

        contentView.dataset.previewKind = "binary";
        contentView.setAttribute("aria-label", `${label} file information`);
        preview.className = "source-binary-preview";
        icon.className = "source-binary-icon";
        icon.textContent = label;
        name.textContent = baseName(file.path);
        details.textContent = file.size === null ? "Binary file" : formatFileSize(file.size);
        preview.append(icon, name, details);
        contentView.replaceChildren(preview);
        resetContentScroll();
    }

    function setLoading() {
        contentView.dataset.previewKind = "loading";
        contentView.setAttribute("aria-label", "Loading file preview");
        contentView.innerHTML = '<div class="source-loading">Loading file…</div>';
        resetContentScroll();
    }

    function renderError(message) {
        contentView.dataset.previewKind = "error";
        contentView.setAttribute("aria-label", "File preview error");
        const error = document.createElement("div");
        error.className = "source-error";
        error.textContent = message;
        contentView.replaceChildren(error);
        resetContentScroll();
    }

    function highlightSource(source, language) {
        const prism = window.Prism;
        const grammar = prism && prism.languages ? prism.languages[language] : null;

        if (!prism || !grammar) {
            return source.split("\n").map(escapeHtml);
        }

        const lines = [""];
        const highlighted = prism.tokenize(source, grammar);

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

    function previewType(path) {
        const extension = fileExtension(path);

        switch (extension) {
            case "cc":
            case "h":
                return {kind: "source", label: "C++", language: "cpp"};
            case "gltf":
                return {kind: "source", label: "JSON", language: "json"};
            case "webp":
                return {kind: "image", label: "WEBP"};
            case "bin":
                return {kind: "binary", label: "BIN"};
            default:
                return {kind: "file", label: extension ? extension.toUpperCase() : "FILE"};
        }
    }

    function fileDescription(file) {
        const preview = previewType(file.path);
        const type = preview.kind === "source" && preview.language === "json"
            ? "glTF JSON"
            : preview.kind === "image"
                ? "WebP image"
                : preview.kind === "binary"
                    ? "Binary data"
                    : preview.label;

        return file.size === null ? type : `${type} · ${formatFileSize(file.size)}`;
    }

    function updateUrl() {
        const url = new URL(window.location.href);

        if (selectedFile) {
            url.searchParams.set("file", selectedFile);
        }
        else {
            url.searchParams.delete("file");
        }

        if (currentDirectory) {
            url.searchParams.set("path", currentDirectory);
        }
        else {
            url.searchParams.delete("path");
        }

        window.history.replaceState({}, "", url);
    }

    function directoryExists(path) {
        if (!path) {
            return true;
        }

        const prefix = `${path}/`;
        return [...files.keys()].some((filePath) => filePath.startsWith(prefix));
    }

    function sourceUrl(path) {
        const encodedPath = path.split("/").map(encodeURIComponent).join("/");
        return new URL(`source/${encodedPath}`, window.location.href);
    }

    function normalizePath(value) {
        if (typeof value !== "string") {
            return "";
        }

        return value
            .replaceAll("\\", "/")
            .split("/")
            .filter((part) => part && part !== ".")
            .join("/");
    }

    function parentDirectory(path) {
        const slash = path.lastIndexOf("/");
        return slash === -1 ? "" : path.slice(0, slash);
    }

    function baseName(path) {
        const slash = path.lastIndexOf("/");
        return slash === -1 ? path : path.slice(slash + 1);
    }

    function fileExtension(path) {
        const name = baseName(path);
        const dot = name.lastIndexOf(".");
        return dot === -1 ? "" : name.slice(dot + 1).toLowerCase();
    }

    function compareNames(left, right) {
        return left.name.localeCompare(right.name, undefined, {numeric: true, sensitivity: "base"});
    }

    function formatFileSize(bytes) {
        if (bytes < 1024) {
            return `${bytes} B`;
        }

        const units = ["KB", "MB", "GB"];
        let value = bytes;
        let unit = "B";

        for (const candidate of units) {
            value /= 1024;
            unit = candidate;

            if (value < 1024) {
                break;
            }
        }

        const precision = value >= 100 || Number.isInteger(value) ? 0 : 1;
        return `${value.toFixed(precision)} ${unit}`;
    }

    function resetContentScroll() {
        contentView.scrollTop = 0;
        contentView.scrollLeft = 0;
    }

    function escapeHtml(value) {
        return value
            .replaceAll("&", "&amp;")
            .replaceAll("<", "&lt;")
            .replaceAll(">", "&gt;")
            .replaceAll('"', "&quot;");
    }
})();
