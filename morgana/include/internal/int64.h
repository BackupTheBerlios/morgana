#ifndef INT64_H
#define INT64_H


class uint64{
        public:
          int64(const int64&i){_hi=i._hi;_lo=i._lo;}
          int64(){_hi=_lo=0;}
          
          uint32 hi(){return _hi;}
          uint32 lo(){return _lo;}
          
          uint64 swab()
#ifdef HAVE_NAMEDRETURN
          	return ret(*this);{}
#else
                {uint64 ret(*this);return ret;}
#endif
          
          uint64& operator=(const uint64&i){_hi=i._hi;_lo=i._lo;return*this;}
          uint64& operator=(uint32 i){_hi=0;_lo=i;return*this;}
          //uint64& operator+=(const uint64&i){????? return*this;}
          bool operator==(const uint64&i){return _hi==i._hi && _lo==i._lo;}
          bool operator==(uint32 i){if(_hi)return false;return _lo==i;}
          bool operator==(sint32 i){if(_hi)return false;return _lo==i;}

#warning int64 classes yet not readily programmed, maybe program does not work
          
        private:
          uint32 _hi,_lo;
};

class sint64{
};
typedef sint64 int64;
          
#endif //INT64_H
