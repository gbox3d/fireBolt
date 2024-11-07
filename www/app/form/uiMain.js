// import * as THREE from 'three';



async function _updateSensorData(_rootElm) {
    const _devid = "671a559487ce959f910ddd5a"

    //update temperature
    try {
        const response = await fetch('/api/logs/detail', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify({ id:_devid }) // id를 요청 바디로 전달
        });

        if (!response.ok) {
            throw new Error(`HTTP error! Status: ${response.status}`);
        }

        const log = await response.json();
        console.log(log);

        const temper = _rootElm.querySelector('#temper-value');
        temper.innerHTML = `Temperature : ${log.data.temperature} C`;

        const date_panel = _rootElm.querySelector('#date-panel');
        date_panel.innerHTML = `${log.data.rdate}`;

        setTimeout(()=> {
            _updateSensorData(_rootElm);
        }, 10000);

        // const detail = document.querySelector('#detail-view-textarea');
        // detail.value = log;

        // 필요한 경우 모달을 열고 로그 내용을 표시
        // messageModal.show(log);
    } catch (error) {
        console.error('Error fetching log:', error);
    }
}

export default async function (_Context) {

    const _htmlText = `
    <div class="ui-view w3-container">
        <div class="w3-panel w3-blue">
            <h2 id="date-panel" >  </h2>
        </div>
        <div class="w3-panel w3-pink" >
            <h2 id="temper-value" >  </h2>
        </div>
    </div>
    `;

    const parser = new DOMParser();
    const htmlDoc = parser.parseFromString(_htmlText, 'text/html');
    const _rootElm = htmlDoc.querySelector('.ui-view');

    _Context.body_container.appendChild(_rootElm);


    _updateSensorData(_rootElm);
    
    console.log('complete setup uiMain');

    return {
        element: _rootElm,
        // menubar: _menuBar,
        // objViewer: objViewer
    }

}