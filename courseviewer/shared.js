// ES6 TypedArray.slice, only implemented by default in firefox
var TypedArrays = [Uint8Array, Uint8ClampedArray, Int16Array, Uint16Array, Int32Array, Uint32Array, Float32Array, Float64Array];
for(var i in TypedArrays){
	if(!TypedArrays[i].prototype.slice){
		TypedArrays[i].prototype.slice = function(start, end){
			var ret = new this.constructor(end - start);
			var j = 0;
			for(var i = start; i < end; i++){
				ret[j] = this[i];
				j++;
			}
			return ret;
		}
	}
}

// todo: use builtin DataView with ArrayBuffer instead of all this crap
// integer management
function SWAP16(i){
	return ((i & 0xFF00) >> 8) | ((i & 0xFF) << 8);
}

function SWAP32(i){
	return (((i & 0xFF000000) >>> 24) |
	       ((i & 0x00FF0000) >>>   8) |
		   ((i & 0x0000FF00) <<    8) |
		   ((i & 0x000000FF) <<   24)) >>> 0;
}

function unsign32(i){ // unsign js s32
	return i >>> 0;
}

function sign16(i){ // sign u16
	if(i & 0x8000) return (i | 0xFFFF0000);
	return i;
}

function sign8(i){ // sign u8
	if(i & 0x80) return (i | 0xFFFFFF00);
	return i;
}

// fetch values from byte arrays:

function getS8(source, offset){
	return sign8(source[offset]);
}

function getU16BE(source, offset){
	return ((source[offset]     << 8) |
	       (source[offset + 1]));
}

function getS16BE(source, offset){
	return sign16(getU16BE(source, offset));
}

function getS32BE(/*Uint8Array*/ source, offset){
	return (source[offset]     << 24) |
	       (source[offset + 1] << 16) |
	       (source[offset + 2] <<  8) |
		   (source[offset + 3]      );
}

function getU32BE(/*Uint8Array*/ source, offset){
	return unsign32(getS32BE(source, offset));
}

function getStringZ(source, offset){
	var ret = "";
	for(var i = 0; source[offset+i]; i++){
		ret += String.fromCharCode(source[offset+i]);
	}
	return ret;
}

function getString(source, offset, length){
	var ret = "";
	for(var i = 0; i < length; i++){
		ret += String.fromCharCode(source[offset+i]);
	}
	return ret;
}

// u8arraytos16array
function Uint8ToInt16ArrayBE(src){
	var ret = [];
	for(var i = 0; i < src.length; i += 2){
		ret.push(getS16BE(src, i));
	}
	return new Int16Array(ret);
}

function Uint8ToUint16ArrayBE(src){
	var ret = [];
	for(var i = 0; i < src.length; i += 2){
		ret.push(getU16BE(src, i));
	}
	return new Uint16Array(ret);
}

// mio0 decoder
function mio0decode(/*Uint8Array*/ src, srcOffset){
	var dest = [];
	var destLen = getU32BE(src, srcOffset + 0x04);
	var controlPos = 0x10; // control bits section
	var dictPos = getU32BE(src, srcOffset + 0x08); // dictionary section
	var dataPos = getU32BE(src, srcOffset + 0x0C); // data section
	var controlBits = 0;
	var controlExaust = 0;
	for(var destPos = 0; destPos < destLen;){
		if(!controlExaust){ // current controlPos bitfield is exhausted, load next
			controlBits = getS32BE(src, srcOffset + controlPos);
			controlExaust = 32;
			controlPos += 4;
		}
		if(controlBits >= 0){ // if upper bit of controlPos word is unset, use dictionary to copy data:
			var dictPair = getU16BE(src, srcOffset + dictPos);
			var length = (dictPair >> 12) + 3; // upper 4 bits + 3 = length (match maxlength = 18, minlength = 3)
			var relOffset = destPos - (dictPair & 0x0FFF) - 1; // index - lower 12 bits (relative negative offset of data to copy + 1)
			for(var j = 0; j < length; j++){
				dest[destPos] = dest[relOffset];
				relOffset++;
				destPos++;
			}
			dictPos += 2;
		} else { // upper bit of controlPos is 1, copy single byte from dataPos
			dest[destPos] = src[srcOffset + dataPos];
			dataPos++;
			destPos++;
		}
		controlBits <<= 1; // next bit
		controlExaust--;
	}
	return dest;
}

// array swap conversions:
function Uint16ArrayBE(arg){ // LE u16 array -> BE u16 array
	var arr = new Uint16Array(arg);
	for(var i = 0; i < arr.length; i++){
		arr[i] = SWAP16(arr[i]); 
	}
	return arr;
}

function Int16ArrayBE(arg){ // LE s16 array -> BE s16 array
	var arr = new Int16Array(arg);
	for(var i = 0; i < arr.length; i++){
		arr[i] = SWAP16(arr[i]); 
	}
	return arr;
}

// filereader instance and result event for file inputs
function BinInput(/*id or dom object*/ fileElement, callback){
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