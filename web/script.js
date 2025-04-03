//const socket = new WebSocket('ws://109.195.83.82:3001');
const socket = new WebSocket('ws://localhost:3001');
const editor = CodeMirror.fromTextArea(document.getElementById('codeEditor'), {
    lineNumbers: true,
    theme: 'material-darker',
    mode: 'text/x-c++src',
    indentUnit: 4,
    matchBrackets: true,
    autoCloseBrackets: true,
    extraKeys: {
        "Ctrl-Space": "autocomplete",
        "Tab": function(cm) {
            cm.replaceSelection("    ");
        }
    },
    gutters: ["CodeMirror-linenumbers", "breakpoints"]
});

const boardId = sessionStorage.getItem("boardId");
const username = sessionStorage.getItem("username");


function sendText() {
    if (socket.readyState === WebSocket.OPEN) {
        const data = { action: "connection", board: boardId };
        socket.send(JSON.stringify(data));
        console.log("Отправлено:", data);
    }
}

socket.onopen = function() {
    console.log("WebSocket соединение установлено.");
    sendText();
};

let version = 0;
socket.addEventListener("message", function(event) {
    const data = JSON.parse(event.data);
    if (data.action === "update" && data.version > version) {
        const cursor = editor.getCursor();
        if (data.content !== editor.getValue()) {
            editor.setValue(data.content);
        }
        editor.setCursor(cursor);
        version = data.version;
    }
});

let lastSent = '';
        editor.on('change', (cm, change) => {
            const content = cm.getValue();
            if (content !== lastSent && socket.readyState === WebSocket.OPEN) {
                lastSent = content;
                
                const data = {
                    action: "changes",
                    content: content,
                    version: version,
                    board: boardId
                };
                socket.send(JSON.stringify(data));
                
            }
        });

socket.onerror = function(error) {
    console.error('WebSocket Error:', error);
};

socket.onclose = function(event) {
    console.log('Connection closed, reconnecting...');
    setTimeout(() => window.location.reload(), 1000);
};

async function runCode() {
    const loader = document.getElementById('loader');
    const output = document.getElementById('output');
    const button = document.querySelector('.button');
    try {
        loader.style.display = 'block';
        button.disabled = true;
        output.textContent = 'Компиляция...\n';
        const code = editor.getValue();
        const data = { action: "compile", content: code };
        socket.send(JSON.stringify(data));
    } catch (error) {
        output.textContent += 'Ошибка компиляции:\n' + error.message;
    } finally {
        loader.style.display = 'none';
        button.disabled = false;
    }
}

socket.addEventListener("message", function(event) {
    const response = JSON.parse(event.data);
    if (response.action === "compile_result") {
        document.getElementById('output').textContent = response.content;
    }
});
