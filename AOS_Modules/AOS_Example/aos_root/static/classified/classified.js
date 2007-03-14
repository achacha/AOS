function getHttpXml(id, state) {
  var xmlHttpReq = false;
  var self = this;
  // Mozilla/Safari
  if (window.XMLHttpRequest) {
    self.xmlHttpReq = new XMLHttpRequest();
  }
  // IE
  else if (window.ActiveXObject) {
    self.xmlHttpReq = new ActiveXObject("Microsoft.XMLHTTP");
  }
  
  var urlGet = id;
  switch(state) {
    case 0 : urlGet = urlGet + "_off.xml?ajax=1"; break;
	case 1 : urlGet = urlGet + "_on.xml?ajax=1"; break;
	default: alert("Error in component state"); return;
  }
  //alert(urlGet);
  self.xmlHttpReq.open('GET', urlGet, true);
  self.xmlHttpReq.onreadystatechange = function() {
      if (self.xmlHttpReq.readyState == 4) {
          insertInto(id, self.xmlHttpReq.responseText);
      }
  }
  self.xmlHttpReq.send(null);
  return false;
}

function insertInto(id, data){
  document.getElementById(id).innerHTML = data;
}
