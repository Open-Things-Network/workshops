function Decoder(bytes, port) {
  var decoded = {};
  var result = "";
  for (var i = 0; i < bytes.length; i++) {
    result += String.fromCharCode(parseInt(bytes[i]));
  }
  decoded.msg = result;
  return decoded;
}