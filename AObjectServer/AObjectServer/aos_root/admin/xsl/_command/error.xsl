<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:fo="http://www.w3.org/1999/XSL/Format">
  <xsl:output method="html"/>
  <xsl:template match="/">
    <html>
      <head>
        <title>Modify redirect page</title>
      </head>
      <body>
        
        <a href="/admin?command=list">Object list</a> |
        <a href="/">Home</a><br/>
        <hr/>
        <br/>

        <h3 color="red">Error has occured</h3>
        <div>
          <xsl:value-of select="/admin/error"/>
        </div>
      </body>
    </html>
  </xsl:template>
</xsl:stylesheet>
