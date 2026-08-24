
(function () {
    "use strict";

    const ICONS = {
        info: "i",
        success: "\u2713",
        error: "\u2715",
        support: "?"
    };

    const stacks = {}; // position -> container element

    function getStack(position) {
        if (stacks[position]) return stacks[position];

        const el = document.createElement("div");
        el.className = "cef-notify-stack";
        el.setAttribute("data-position", position);
        document.body.appendChild(el);

        stacks[position] = el;
        return el;
    }

    function slideDirFor(position) {
        if (position.endsWith("right")) return "right";
        if (position.endsWith("left")) return "left";
        if (position.startsWith("top")) return "up";
        return "down";
    }

    function showToast(options) {
        const title = options.title;
        const message = options.message || "";
        const type = options.type || "info";
        const duration = typeof options.duration === "number" ? options.duration : 4000;
        const position = options.position || "top-left";

        const stack = getStack(position);
        const slide = slideDirFor(position);

        const toast = document.createElement("div");
        toast.className = "cef-toast";
        toast.setAttribute("data-type", type);
        toast.setAttribute("data-slide", slide);
        if (title) toast.setAttribute("data-has-title", "");

        toast.innerHTML =
            '<div class="cef-toast-bar"><div class="cef-toast-bar-fill" style="height:100%"></div></div>' +
            '<div class="cef-toast-icon">' + (ICONS[type] || ICONS.info) + "</div>" +
            '<div class="cef-toast-divider"></div>' +
            '<div class="cef-toast-body">' +
            (title ? '<div class="cef-toast-title"></div>' : "") +
            '<div class="cef-toast-message"></div>' +
            "</div>";

        if (title) {
            toast.querySelector(".cef-toast-title").innerText = title;
        }
        toast.querySelector(".cef-toast-message").innerText = message;

        stack.appendChild(toast);

        requestAnimationFrame(function () {
            toast.classList.add("show");
        });

        // progress bar countdown
        const fill = toast.querySelector(".cef-toast-bar-fill");
        requestAnimationFrame(function () {
            fill.style.transition = "height " + duration + "ms linear";
            fill.style.height = "0%";
        });

        const hideTimer = setTimeout(function () {
            toast.classList.remove("show");
        }, duration);

        const removeTimer = setTimeout(function () {
            toast.remove();
        }, duration + 250);

        toast.addEventListener("click", function () {
            clearTimeout(hideTimer);
            clearTimeout(removeTimer);
            toast.classList.remove("show");
            setTimeout(function () {
                toast.remove();
            }, 200);
        });
    }

    function handleCefNotification(e) {
        const detail = e.detail;
        if (typeof detail !== "string") return;

        let parsed;
        try {
            parsed = JSON.parse(detail);
        } catch (err) {
            console.error("[CEF Notify] Failed to parse payload:", err, detail);
            return;
        }

        let title, message, type, duration;
        if (Array.isArray(parsed)) {
            title = parsed[0];
            message = parsed[1];
            type = parsed[2];
            duration = parsed[3];
        } else {
            title = parsed.title;
            message = parsed.message;
            type = parsed.type;
            duration = parsed.duration;
        }

        showToast({
            title: title || undefined,
            message: message || "",
            type: type || "info",
            duration: typeof duration === "number" ? duration : 4000,
            position: "top-left"
        });
    }

    window.addEventListener("cef_ui_notification", handleCefNotification);


    window.CefNotify = { show: showToast };

    window.testCefNotify = function (title, message, type, duration) {
        window.dispatchEvent(
            new CustomEvent("cef_ui_notification", {
                detail: JSON.stringify([title, message, type || "success", duration || 3000])
            })
        );
    };
})();