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
        pathLabel.textContent = `demos/001-physics-sandbox/${fileName}`;
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
        const keywords = new Set([
            "alignas", "alignof", "and", "asm", "auto", "break", "case", "catch", "class", "concept",
            "const", "consteval", "constexpr", "constinit", "continue", "co_await", "co_return", "co_yield",
            "decltype", "default", "delete", "do", "else", "enum", "explicit", "export", "extern", "for",
            "friend", "goto", "if", "inline", "mutable", "namespace", "new", "noexcept", "not", "operator",
            "override", "private", "protected", "public", "requires", "return", "sizeof", "static", "struct",
            "switch", "template", "this", "throw", "try", "typedef", "typename", "union", "using", "virtual",
            "volatile", "while"
        ]);
        const types = new Set([
            "bool", "char", "char8_t", "char16_t", "char32_t", "double", "float", "int", "long", "short",
            "signed", "unsigned", "void", "wchar_t", "size_t", "nullptr", "true", "false"
        ]);

        let inBlockComment = false;

        return source.split("\n").map((line) => {
            let index = 0;
            let output = "";

            while (index < line.length) {
                if (inBlockComment) {
                    const end = line.indexOf("*/", index);
                    const stop = end === -1 ? line.length : end + 2;
                    output += token("comment", line.slice(index, stop));
                    index = stop;
                    inBlockComment = end === -1;
                    continue;
                }

                if (line.startsWith("//", index)) {
                    output += token("comment", line.slice(index));
                    break;
                }

                if (line.startsWith("/*", index)) {
                    const end = line.indexOf("*/", index + 2);
                    const stop = end === -1 ? line.length : end + 2;
                    output += token("comment", line.slice(index, stop));
                    index = stop;
                    inBlockComment = end === -1;
                    continue;
                }

                const character = line[index];

                if (character === "#" && line.slice(0, index).trim() === "") {
                    const match = line.slice(index).match(/^#[A-Za-z_]+/);
                    if (match) {
                        output += token("preprocessor", match[0]);
                        index += match[0].length;
                        continue;
                    }
                }

                if (character === '"' || character === "'") {
                    const quote = character;
                    let end = index + 1;
                    let escaped = false;

                    while (end < line.length) {
                        const current = line[end];
                        if (!escaped && current === quote) {
                            end += 1;
                            break;
                        }
                        escaped = !escaped && current === "\\";
                        if (current !== "\\") {
                            escaped = false;
                        }
                        end += 1;
                    }

                    output += token("string", line.slice(index, end));
                    index = end;
                    continue;
                }

                if (/[A-Za-z_]/.test(character)) {
                    const match = line.slice(index).match(/^[A-Za-z_][A-Za-z0-9_]*/)[0];
                    if (keywords.has(match)) {
                        output += token("keyword", match);
                    }
                    else if (types.has(match)) {
                        output += token("type", match);
                    }
                    else {
                        output += escapeHtml(match);
                    }
                    index += match.length;
                    continue;
                }

                if (/\d/.test(character) || (character === "." && /\d/.test(line[index + 1] || ""))) {
                    const match = line.slice(index).match(/^(?:0[xX][0-9A-Fa-f']+|0[bB][01']+|(?:\d[\d']*\.?[\d']*|\.\d[\d']*)(?:[eE][+-]?\d+)?)[uUlLfF]*/);
                    if (match) {
                        output += token("number", match[0]);
                        index += match[0].length;
                        continue;
                    }
                }

                output += escapeHtml(character);
                index += 1;
            }

            return output;
        });
    }

    function token(type, value) {
        return `<span class="syntax-${type}">${escapeHtml(value)}</span>`;
    }

    function escapeHtml(value) {
        return value
            .replaceAll("&", "&amp;")
            .replaceAll("<", "&lt;")
            .replaceAll(">", "&gt;")
            .replaceAll('"', "&quot;");
    }
})();
