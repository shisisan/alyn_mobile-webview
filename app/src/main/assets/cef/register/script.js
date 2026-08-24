// GPT GENERATED HEHE

if (typeof Cef === "undefined") {
    window.Cef = {
        sendEvent: function (eventName, dataJson) {
            console.log(
                "[MOCK CEF OUTGOING]",
                eventName,
                JSON.parse(dataJson)
            );
        }
    };
}

if (window.AndroidCefBridge) {
    window.Cef.sendEvent = function (eventName, dataJson) {
        window.AndroidCefBridge.sendEvent(eventName, dataJson);
    };

    console.log("[CEF Client] AndroidCefBridge detected, using real bridge.");
} else {
    console.log("[CEF Client] AndroidCefBridge not found, using mock bridge.");
}


function handleLogin(event) {
    event.preventDefault();

    const username =
        document.getElementById("username").value.trim();

    const password =
        document.getElementById("password").value;

    const remember =
        document.getElementById("remember").checked;


    if (username.length < 3) {
        showError("Please enter a valid username.");
        return;
    }

    if (password.length < 1) {
        showError("Please enter your password.");
        return;
    }


    hideError();

    setLoadingState(true);

    const payload = [
        username,
        password,
        remember
    ];


    console.log(
        "[CEF Client] Sending login request..."
    );

    // !! Must be string
    Cef.sendEvent(
        "login_submit",
        JSON.stringify(payload)
    );
}

function handleCancel() {

    console.log(
        "[CEF Client] User cancelled login."
    );

    // !! Must be string
    Cef.sendEvent(
        "login_cancel",
        JSON.stringify(["cancel"])
    );
}


function togglePassword() {

    const password =
        document.getElementById("password");

    const button =
        document.getElementById("password-toggle");


    if (password.type === "password") {

        password.type = "text";

        button.innerText = "HIDE";

    } else {

        password.type = "password";

        button.innerText = "SHOW";
    }
}


function showError(message) {

    const banner =
        document.getElementById("error-banner");

    const messageElement =
        document.getElementById("error-message");


    messageElement.innerText = message;

    banner.classList.remove("hidden");
}


function hideError() {

    document
        .getElementById("error-banner")
        .classList.add("hidden");
}

function setLoadingState(isLoading) {

    const button =
        document.getElementById("login-button");

    const text =
        document.getElementById("login-button-text");

    const spinner =
        document.getElementById("login-spinner");


    if (isLoading) {

        text.innerText = "AUTHENTICATING";

        spinner.classList.remove("hidden");

        button.disabled = true;

    } else {

        text.innerText = "LOGIN";

        spinner.classList.add("hidden");

        button.disabled = false;
    }
}

function onLoginResponse(data) {

    setLoadingState(false);


    try {

        const success =
            data[0];

        const message =
            data[1] || "";


        if (!success) {

            showError(
                message ||
                "Invalid username or password."
            );

            return;
        }


        hideError();


        console.log(
            "[CEF Client] Login successful."
        );


        /*
         * Tell the game that authentication
         * was completed.
         */

        Cef.sendEvent(
            "login_success",
            JSON.stringify(["success"])
        );


    } catch (error) {

        console.error(
            "[CEF Client] Failed to handle login response:",
            error
        );


        showError(
            "An unexpected server response was received."
        );
    }
}

window.addEventListener("login_response", function (e) {
    try {
        const data = JSON.parse(e.detail);
        onLoginResponse(data);
    } catch (err) {
        console.error("[CEF Client] Failed to parse login_response payload:", err);
    }
});

document.addEventListener(
    "keydown",
    function (event) {

        if (event.key === "Enter") {

            const activeElement =
                document.activeElement;


            if (
                activeElement &&
                activeElement.tagName === "INPUT"
            ) {

                const form =
                    document.getElementById("login-form");

                if (form) {
                    form.requestSubmit();
                }
            }
        }


        if (event.key === "Escape") {

            handleCancel();
        }
    }
);