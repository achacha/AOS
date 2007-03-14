function getHttpXml(tooltip_name) {
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
  
  var urlGet = "tooltip_"+tooltip_name+".xml";
  self.xmlHttpReq.open('GET', urlGet, true);
  self.xmlHttpReq.onreadystatechange = function() {
      if (self.xmlHttpReq.readyState == 4) {
          showTooltip(self.xmlHttpReq.responseText);
      }
  }
  self.xmlHttpReq.send(null);
  return false;
}

function showTooltip(data)
{
	return overlib(data, DELAY, 800, WIDTH, 110, HAUTO, VAUTO);
}