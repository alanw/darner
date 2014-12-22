#ifndef __DARNER_REQUEST_HPP__
#define __DARNER_REQUEST_HPP__

#include <string>

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "darner/queue/queue.h"

namespace darner {

struct request
{
   enum request_type
   {
      RT_STATS     = 1,
      RT_VERSION   = 2,
      RT_DESTROY   = 3,
      RT_FLUSH     = 4,
      RT_FLUSH_ALL = 5,
      RT_SET       = 6,
      RT_GET       = 7
   };

   request_type type;
   std::string queue;

   std::string queue_name_;
   queue::size_type queue_limit_;

   std::string queue_name()
   {
      if (queue_name_.empty())
         parse_name();
      return queue_name_;
   }

   queue::size_type queue_limit()
   {
      if (queue_name_.empty())
         parse_name();
      return queue_limit_;
   }

   void parse_name()
   {
      std::vector<std::string> name_limit;
      boost::split(name_limit, queue, boost::is_any_of(":"), boost::token_compress_on);
      if (name_limit.size() <= 1 || name_limit[1].empty())
      {
         queue_name_ = queue;
         queue_limit_ = 0; // unbounded
      }
      else
      {
         queue_name_ = name_limit[0];
         queue_limit_ = boost::lexical_cast<queue::size_type>(name_limit[1]);
      }
   }

   size_t num_bytes;
   bool get_open;
   bool get_peek;
   bool get_close;
   bool get_abort;
   bool set_sync;
   size_t wait_ms;
};

struct request_grammar : boost::spirit::qi::grammar<std::string::const_iterator>
{
   request_grammar();
   request req;
   boost::spirit::qi::rule<std::string::const_iterator, std::string()> key_name;
   boost::spirit::qi::rule<std::string::const_iterator> stats, version, destroy, flush, flush_all, set_option, set, get_option, get, start;
};

// grammar are expensive to construct.  to be thread-safe, let's make one grammar per thread.
class request_parser
{
public:

   bool parse(request& req, std::string::const_iterator begin, std::string::const_iterator end)
   {
      grammar_.req = request();
      bool success = boost::spirit::qi::parse(begin, end, grammar_) && (begin == end);
      if (success)
         req = grammar_.req;

      return success;
   }

   template <class Sequence>
   bool parse(request& req, const Sequence& seq)
   {
      return parse(req, seq.begin(), seq.end());
   }

private:

   request_grammar grammar_;
};

} // darner

#endif // __DARNER_REQUEST_HPP__
