<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method="html" indent="yes"/>
  <xsl:template match="/root">
    <html>
      <head>
        <title>Global variables</title>
      </head>
      <body>
        <span style="font-size:16pt">Results from <span style="font-weight:300; color:red">select * from global</span></span>
        <br/>
        <hr/>
        <br/>
        <span style="position:absolute; width:100; left:0; font-size:14pt; font-weight:bold; background-color:#ffffff; color:#003333;">Name</span>
        <span style="position:absolute; width:500; left:100; font-size:14pt; font-weight:bold; background-color:#ffffff; color:#003333;">Value</span>
        <br/>
        <div style="float:left"><xsl:apply-templates select="row"/></div>
      </body>
    </html>
  </xsl:template>

  <xsl:template match="row">
    <span style="position:absolute; width:100; left:0; background-color:#cccccc; color:#000000; border-color:#000000; border:1; border-style:solid;"><xsl:value-of select="name"/></span>
    <span style="position:absolute; width:500; left:100; background-color:#cfcfcf; color:#000000; border-color:#000000; border:1; border-style:solid;"><xsl:value-of select="value"/></span>
    <br/>
  </xsl:template>

</xsl:stylesheet>
