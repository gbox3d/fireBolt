function BufferLoader(context,root_path, urlList, callback) {
	this.context = context;

    this.root_path = root_path;
    this.urlList = urlList;

	this.onload = callback;
	this.bufferList = new Array();
	this.loadCount = 0;
}

BufferLoader.prototype.loadBuffer = function(url, index) {
	var request = new XMLHttpRequest();
	request.open("GET", this.root_path + url, true);
	request.responseType = "arraybuffer";
	var loader = this;
	request.onload = function() {
		loader.context.decodeAudioData(request.response, function(buffer) {
			if (!buffer) {
				alert('error decoding file data: ' + url);
				return;
			}
			console.log('success load : ' + url);
            buffer.url = url;
			loader.bufferList[index] = buffer;
			if (++loader.loadCount == loader.urlList.length)
				loader.onload(loader.bufferList);
		}, function(error) {
			console.error('decodeAudioData error', error);
		});
	}
	request.onerror = function() {
		alert('BufferLoader: XHR error');
	}
	request.send();
}
BufferLoader.prototype.load = function() {
	for (var i = 0; i < this.urlList.length; ++i)
		this.loadBuffer(this.urlList[i], i);
}


