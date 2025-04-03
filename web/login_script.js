//const socket = new WebSocket('ws://192.168.1.55:3001'); //localhost
//const socket = new WebSocket('ws://109.195.83.82:3001');
const socket = new WebSocket('ws://localhost:3001');
document.getElementById('loginForm').addEventListener('submit', function(e) {
    e.preventDefault();
    
    const boardId = document.getElementById('boardSelect').value;
    const password = document.getElementById('boardPassword').value;
    const username = document.getElementById('username').value.trim();
    
    if (!username) {
        showError('Введите ваш никнейм!');
        return;
    }
    if (!password) {
        showError('Введите ваш password!');
        return;
    }
    sendText();
});

socket.onopen = function(event) {
    console.log('WebSocket connection opened');
};

socket.onmessage = function(event) {
    try {
        const data = JSON.parse(event.data);

        if (data.status === "success") {
            const boardId = data.board; 
            const username = data.user; 
            console.log(boardId);
            console.log(username);

            sessionStorage.setItem("boardId", boardId);
            sessionStorage.setItem("username", username);
            window.location.href = `editor.html`;

        } else {
            console.error("Ошибка: Неверный формат сообщения от сервера", data);
            alert("Ошибка: Не удалось получить данные для перенаправления.");
        }
    } catch (error) {
        console.error("Ошибка при обработке JSON:", error);
        alert("Ошибка: Не удалось обработать ответ от сервера.");
    }
};

socket.onclose = function(event) {
    console.log('WebSocket connection closed');
};

socket.onerror = function(error) {
    console.error('WebSocket error:', error);
};

function sendText() {
    const username = document.getElementById('username').value;
    const password = document.getElementById('boardPassword').value;
    const board = document.getElementById('boardSelect').value;

    const data = {
        action: "message",
        user: username,
        board: board,
        auth: {
            password: password
        }
    };

    socket.send(JSON.stringify(data));
}

function showError(message) {
    document.getElementById('errorMessage').textContent = message;
}
