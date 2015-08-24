function SWAP16(i){
	return ((i & 0xFF00) >> 8) | ((i & 0xFF) << 8);
}

function SWAP32(i){
	return (((i & 0xFF000000) >>> 24) |
	       ((i & 0x00FF0000) >>>  8) |
		   ((i & 0x0000FF00) <<  8) |
		   ((i & 0x000000FF) << 24)) >>> 0;
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
function mio0decode(/*Uint8Array*/ src, src_offset){
	var dest = [];
	var target_size = getU32BE(src, src_offset + 0x04);
	var offset1 = getU32BE(src, src_offset + 0x08);
	var offset2 = getU32BE(src, src_offset + 0x0C);
	var offset0 = 0x10;
	var t2 = 0;
	var t3 = 0;
	var t1 = 0;
	var t0 = 0;
	var bit = 0;
	for(var i = 0; i < target_size;){
		if(bit == 0){
			t0 = getS32BE(src, src_offset + offset0);
			bit = 32;
			offset0 += 4;
		}
		if(t0 >= 0){
			t2 = getU16BE(src, src_offset + offset1);
			offset1 += 2;
			t3 = t2 >> 12;
			t2 &= 0x0FFF;
			t1 = i - t2;
			t3 += 3;
			while(t3 != 0){
				t2 = dest[t1 - 1];
				t3--;
				t1++;
				dest[i] = t2;
				i++;
			}
		} else {
			t2 = src[src_offset + offset2];
			offset2++;
			dest[i] = t2;
			i++;
		}
		t0 <<= 1;
		bit--;
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