import messageModal from 'ideon/messageModal.js';


async function updateDetail(id) {
    try {
        const response = await fetch('/api/logs/detail', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify({ id }) // id를 요청 바디로 전달
        });

        if (!response.ok) {
            throw new Error(`HTTP error! Status: ${response.status}`);
        }

        const log = await response.text();
        // console.log('log:', log);

        const detail = document.querySelector('#detail-view-textarea');
        detail.value = log;

        // 필요한 경우 모달을 열고 로그 내용을 표시
        // messageModal.show(log);
    } catch (error) {
        console.error('Error fetching log:', error);
    }
}

async function _updateList() {
    try {
        // POST 요청을 통해 목록을 가져옵니다.
        const response = await fetch('/api/logs/list', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify({
                page: 1,
                limit: 10
            })
        });

        // 응답이 성공적인지 확인
        if (!response.ok) {
            throw new Error(`HTTP error! Status: ${response.status}`);
        }

        // JSON 데이터로 변환
        const data = await response.json();

        console.log('data : ', data);

        if (data.r == 'ok') {

            // 로그 목록을 화면에 표시
            const logList = document.querySelector('#logList ul');
            logList.innerHTML = ''; // 기존 목록 초기화


            const _list = data.list;
            _list.forEach(element => {
                const listItem = document.createElement('li');
                console.log('element : ', element);

                //title 이 존재하면 title을 표시
                if (element.title) {
                    listItem.innerText = `${element.rdate} : ${element.title}`;
                } else {
                    listItem.innerText = `${element.rdate} : ${element._id}`;
                }

                listItem._id = element._id;

                listItem.addEventListener('click', () => {
                    console.log('click : ', element);
                    // messageModal.show(element);
                    updateDetail({ id: element._id });

                    const logList = document.querySelector('#logList ul');
                    const items = logList.querySelectorAll('li');
                    items.forEach(item => {
                        item.classList.remove('selected');
                    });

                    listItem.classList.add('selected');


                });

                logList.appendChild(listItem);
            });
        }


        // data.logs.forEach(log => {
        //     const listItem = document.createElement('li');
        //     listItem.textContent = `${log.level.toUpperCase()}: ${log.message}`;
        //     listItem.addEventListener('click', () => showDetail(log.id));
        //     logList.appendChild(listItem);
        // });

    } catch (error) {
        console.error('Error fetching log list:', error);
    }

}

async function _delereLog(id) {
    console.log('delete log id : ', id);

    try {
        // POST 요청을 통해 목록을 가져옵니다.
        const response = await fetch('/api/logs/delete', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify({
                id
            })
        });

        // 응답이 성공적인지 확인
        if (!response.ok) {
            throw new Error(`HTTP error! Status: ${response.status}`);
        }

        // JSON 데이터로 변환
        const data = await response.json();

        console.log('data : ', data);

        if (data.r == 'ok') {

            await _updateList();

            alert('delete ok');

        }

    }
    catch (error) {
        console.error('Error fetching log list:', error);
    }
}

async function _updateLog(id, title, content) {
    console.log('update log id : ', id);

    try {
        // POST 요청을 통해 목록을 가져옵니다.
        const response = await fetch('/api/logs/update', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify({
                id,
                update: {
                    title,
                    content
                }
            })
        });

        // 응답이 성공적인지 확인
        if (!response.ok) {
            throw new Error(`HTTP error! Status: ${response.status}`);
        }

        // JSON 데이터로 변환
        const data = await response.json();

        console.log('data : ', data);

        if (data.r == 'ok') {

            await _updateList();

            alert('update ok');

        }

    }
    catch (error) {
        console.error('Error fetching log list:', error);
    }
    
}

async function main() {

    console.log('main start');

    document.querySelector('#btnGetList').addEventListener('click', async () => {

        console.log('btnGetList click');

        _updateList();


    });


    document.querySelector('#btnInsert').addEventListener('click', async () => {

        console.log('btnInsert click');

        try {
            // POST 요청을 통해 목록을 가져옵니다.
            const response = await fetch('/api/logs/insert', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json'
                },
                body: JSON.stringify({
                    title: 'title here',
                    content: 'content here'
                })
            });

            // 응답이 성공적인지 확인
            if (!response.ok) {
                throw new Error(`HTTP error! Status: ${response.status}`);
            }

            // JSON 데이터로 변환
            const data = await response.json();

            console.log('data : ', data);

            if (data.r == 'ok') {

                await _updateList();

                alert('insert ok');

            }

        } catch (error) {
            console.error('Error fetching log list:', error);
        }


    });


    document.querySelector('#btnDel').addEventListener('click', async () => {

        const selected = document.querySelector('#logList ul li.selected');
        if (!selected) {
            alert('선택된 항목이 없습니다.');
            return;
        }
        const id = selected._id;

        _delereLog(id);
    });


    document.querySelector('#btnUpdate').addEventListener('click', async () => {

        const selected = document.querySelector('#logList ul li.selected');
        if (!selected) {
            alert('선택된 항목이 없습니다.');
            return;
        }
        const id = selected._id;

        const _textData = document.querySelector('#detail-view-textarea');

        const _json = JSON.parse(_textData.value);


        console.log('update click : ', _json);

        await _updateLog(id, _json.data.title, _json.data.content);

        _textData.value = '';

        // _updateLog(id, title, content);
    });
}

export default main;