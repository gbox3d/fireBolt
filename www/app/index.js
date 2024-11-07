import uiMainSetup from "./form/uiMain.js";

async function main() {

    // console.log(`THREEJS Version : ${THREE.REVISION} `);
    // console.log(`WebGL Support : ${WEBGL.isWebGL2Available()}`);

    const titleBar = document.querySelector('#title-bar')
    titleBar.innerHTML = 'Sensor Controller';

    try {
        const mainContext = {            
            modalContainer: document.querySelector('.modal-container'),
            body_container: document.querySelector('.body-container'),
        }

        // mainContext.progressBox = progressBoxSetup(mainContext);
        // mainContext.waitModal = waitModalSetup(mainContext);
        // mainContext.messageModal = messageModal(mainContext);

        mainContext.uiMain = await uiMainSetup(mainContext);      

    }
    catch (err) {
        console.error(err);
    }
}



export default main;