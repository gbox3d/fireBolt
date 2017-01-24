/**
 * Created by gunpower on 2015. 12. 31..
 */
opentype.Font.prototype.getTextWidth = function (option) {

    var fontSize = option.fontSize;
    var text = option.text;

    var fontScale = 1 / this.unitsPerEm * fontSize;
    var glyphs = this.stringToGlyphs(text);
    var string_width = 0;

    for(var i=0;i<glyphs.length;i++) {

        string_width += glyphs[i].advanceWidth * fontScale;

        var kerningValue = 0
        if(i < glyphs.length-1) {
            kerningValue = this.getKerningValue(glyphs[i], glyphs[i + 1]);
        }
        string_width += kerningValue;
        //console.log(string_width);
        //-----------
    }
    //--------
    return string_width;
    //----------
}