#include <q_interval.h>

QInterval::QInterval ( const BWTPosition& begin,
                       const BWTPosition& end )
{
  _begin = begin;
  _end = end;
}

QInterval::QInterval ( const QInterval& other )
{
  _begin = other.get_begin( );
  _end = other.get_end( );
}

QInterval::~QInterval ( )
{
}

BWTPosition QInterval::get_size ( ) const
{
  return ( _begin - _end );
}

BWTPosition QInterval::get_begin ( ) const
{
  return _begin;
}

BWTPosition QInterval::get_end ( ) const
{
  return _end;
}

bool QInterval::operator== (const QInterval& rhs ) const
{
  return (this->_begin == rhs._begin &&
          this->_end == rhs._end);
}

bool QInterval::operator!= (const QInterval& rhs ) const
{
  return (! (*this == rhs));
}

QInterval& QInterval::operator= ( const QInterval& rhs )
{
  if(this != &rhs)
    {
      this->_begin = rhs._begin;
      this->_end = rhs._end;
    }

  return *this;
}
