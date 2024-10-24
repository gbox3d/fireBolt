#ifndef TONKEY_HPP
#define TONKEY_HPP

#define MAXTOKENS 64


class tonkey
{
private:
    /* data */
    String mTokens[MAXTOKENS];
    int mTokenCount = 0;
    
    
public:
    
    inline  tonkey(/* args */) {}
    inline ~tonkey() {}

    inline void parse(String _strLine) {
        int StringCount = 0;
    // parse command
        while (_strLine.length() > 0)
        {
            int index = _strLine.indexOf(' ');
            if (index == -1) // No space found
            {
                mTokens[StringCount++] = _strLine;
                break;
            }
            else
            {
                mTokens[StringCount++] = _strLine.substring(0, index);
                _strLine = _strLine.substring(index + 1);
            }
        }

        mTokenCount = StringCount;

    }

    inline int getTokenCount() { return mTokenCount; }
    inline String getToken(int _index) { return mTokens[_index]; }

};


#endif // TONKEY_HPP