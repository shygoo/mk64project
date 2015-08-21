function SWAP16(s){
	return ((s & 0xFF00) >> 8) | ((s & 0xFF) << 8);
}

function Uint16ArrayBE(arg){
	return new Uint16Array(arg).map(function(s){return SWAP16(s);});
}

function Int16ArrayBE(arg){
	return new Int16Array(arg).map(function(s){return SWAP16(s);});
}

function BinInput(fileElement, callback){
	if(typeof(fileElement) == "string"){
		fileElement = document.getElementById(fileElement);
	}
	var reader = new FileReader();
	reader.onloadend = function(){
		callback(this.result);
	}
	fileElement.onchange = function(){
		reader.readAsArrayBuffer(this.files[0]);
	}
	this.fileElement = fileElement;
	this.callback    = callback;
}