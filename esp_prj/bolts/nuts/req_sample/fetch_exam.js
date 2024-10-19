import fetch from "node-fetch"

(async function() {

    let _res = await (await (fetch('http://localhost:8080/rest/post/writeFile', {
        method: 'POST',
        body: 'hello post this is node-fetch test',
        headers: {
            'Content-Type': 'text/plain',
            'write-name' : 'test.txt',
            'write-directory' : './uploads'

        } 
    }))).json();

    console.log(_res);


    _res = await (await fetch(`http://localhost:8080/rest/get/filelist?cwd=./`)).json()
    console.log(_res)
})();