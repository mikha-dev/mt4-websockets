#include <map>
#include <set>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

using std::string;
using std::set;

typedef std::map<string, string> mapType;

  class XParams { 

  private:

	  public:  XParams()  {   

	  }

    ~XParams(void) {
      data.clear();
    }

    int Init( string pathCache ) {
      _pathCache = pathCache;

      FILE *file;

      try {
          _load( _pathCache.c_str() );
          return 1;
       } catch (...) {
        remove( pathCache.c_str() );
        return 0;
      }

      return 1;
    }    

    bool Save() {
      if( _pathCache.empty() )
        return false;

      FILE *file;

      if (( file = fopen( _pathCache.c_str(), "w" ) ) != NULL) {
        _dump( file );
        fclose( file );
        return true;
      }    

      return false;
    }    

    bool ClearCache() {
      data.clear();
      return Save();
    }

    void ClearCacheFile( const char * file ) {
      remove( file );
    }

    void Free() {
      data.clear();
    }

    void _load( const char *path ) {
      char *buf;
      std::string s, b;
      int nk = 0;
      char* key;
      char* val;
      int k, v;

      data.clear();

      std::ifstream file(path);

      while (std::getline(file, s)) {
        b.append( s );
        b.append( "&" );
      }

      _tokenize( b, "=&" );
    }    

    void _dump( FILE * file ) {
      std::set<std::string>::iterator si;

      for(mapType::iterator it = data.begin(); it != data.end(); ++it) {
        if(keys.count(it->first))
          fprintf( file, "%s=%s\n", it->first.c_str(), it->second.c_str() );
      } 
    }    

    double to_double( const std::string& s, double def )
     {
       std::istringstream i(s);
       double x;
       if (!(i >> x))
         return def;
       return x;
     } 

	  double Get(const std::string& key, double def )  {   
      string s;
      double d;

		  if( data.count( key ) != 0 )
			  s = data[key];
		  else
			  return def;
      
      d = to_double( s, def );

      return d;
	  } 

    string Get(const std::string& key )  {   
		  if( data.count( key ) != 0 )
			  return data[key];
		  else
			  return "";
	  } 

	  const bool Set(const std::string& key, string val, bool is_save=false)  {   
      if(is_save) {
        keys.insert( key );
        Save();
      }
      data[key] = val;
		  return true;
	  } 

	  const bool Remove( const char* key ) {
		  data.erase( key );

		  return true;
	  }

    std::string Serialize() {
      return string("");
    }

    void _tokenize(const std::string& args, const std::string& delims ) {
      char* tmp = _strdup(args.c_str());
      char* seed = tmp;
      char* key = 0;
      char* val = 0;

      do {
        key = strtok(seed, delims.c_str());
        seed = 0;
        val = strtok(seed, delims.c_str());

        if (key && val) {
          data[key] = val;
        }
      } while (key && val);

      free(tmp);
    }

    string _decrypt( char * ePtr, const char* key ) {
      int len = strlen(ePtr);
      for ( int i = 0, j = 0 ; i < len; i++ ) {
        ePtr[i] = ePtr[i]^key[j++];
        if( j == strlen(key)) 
          j = 0;
      }

      return(string(ePtr));
    }

    void _cvt(char *dest, const char *src)
    {
      const char *p = src;
      char code[3] = {0};
      unsigned long ascii = 0;
      char *end = NULL;

      while(*p)
      {
        if(*p == '%')
        {
          memcpy(code, ++p, 2);
          ascii = strtoul(code, &end, 16);
          *dest++ = (char)ascii;
          p += 2;
        }
        else
          *dest++ = *p++;
      }
    }

    inline bool is_base64(unsigned char c) {
      return (isalnum(c) || (c == '+') || (c == '/'));
    }

    std::string base64_decode(std::string const& encoded_string) {
      const std::string base64_chars = 
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";

      int in_len = encoded_string.size();
      int i = 0;
      int j = 0;
      int in_ = 0;
      unsigned char char_array_4[4], char_array_3[3];
      std::string ret;

      while (in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
        char_array_4[i++] = encoded_string[in_]; in_++;
        if (i ==4) {
          for (i = 0; i <4; i++)
            char_array_4[i] = base64_chars.find(char_array_4[i]);

          char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
          char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
          char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

          for (i = 0; (i < 3); i++)
            ret += char_array_3[i];
          i = 0;
        }
      }

      if (i) {
        for (j = i; j <4; j++)
          char_array_4[j] = 0;

        for (j = 0; j <4; j++)
          char_array_4[j] = base64_chars.find(char_array_4[j]);

        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

        for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
      }

      return ret;
    }

    bool Deserialize( string &args, const char* key ) {
      string s;

      s = base64_decode( args );
      _tokenize( s, "=&" );

      return true;
    }

    private:  

      set<string> keys;
      mapType data; 
      string _pathCache;  
  };  

  static XParams Params;
