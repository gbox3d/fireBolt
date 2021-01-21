#ifndef ESFOS_DRIVER_HPP
#define ESFOS_DRIVER_HPP

namespace esfos
{
    //Serial Parse
    inline void run_driver(char *strBuf=NULL)
    {
        while (Serial.available() > 0)
        {
            // String _strReturn="";
            String _strLine = Serial.readStringUntil('\n');

            if (!strBuf)
            {
                if (_strLine == "<save>")
                {

                    Serial.printf("save ready\n");
                    delay(200);
                    String _fileName = Serial.readStringUntil('\n');
                    Serial.printf("file name %s \n", _fileName.c_str());

                    // String _content = Serial.readStringUntil('\n');
                    // Serial.printf("%s \n",_content.c_str());
                    // Serial.printf("ok \n");

                    File _f = SPIFFS.open(_fileName, "w");
                    if (_f)
                    {
                        delay(200);
                        //Serial.printf("file name %s \n", _fileName.c_str());
                        String _content = Serial.readStringUntil('\n');
                        int _size = _f.write(_content.c_str());

                        Serial.printf("%s , %d byte saved\n", _content.c_str(), _size);

                        _f.close();
                    }
                    else
                    {
                        Serial.printf("file error %s \n", _fileName.c_str());
                        Serial.printf("saved failed \n");
                    }
                }

            }
            else {
                if (_strLine.startsWith("{"))
                {
                    // Serial.println("may be json ?\n");
                    // Serial.println(_strLine.c_str());
                    // _strReturn = _strLine;

                    strcpy(strBuf, _strLine.c_str());

                }

            }




            // return _strReturn;
        }
    }

} // namespace esfos

#endif
