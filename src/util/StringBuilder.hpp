#pragma once

#include <utility>
#include <string>
#include <string_view>
#include <ostream>
#include <variant>

namespace aima::util {
    namespace detail {
        template< typename Char, typename Traits, typename Allocator >
        std::basic_string<Char, Traits, Allocator>
        string( typename std::basic_string<Char, Traits, Allocator>::size_type n ) {
            std::basic_string<Char, Traits, Allocator> s;
            s.reserve( n );
            return s;
        }

        template< typename Char, typename Traits, typename Allocator >
        class StringBuilderBuffer : public std::basic_streambuf<Char, Traits> {
            using String     = std::basic_string<Char, Traits, Allocator>;
            using StringView = std::basic_string_view<Char, Traits>;
        public:
            explicit StringBuilderBuffer( String& string ) : string( string ) {
                this->setp( string.data(), string.data() + string.capacity());
            }

            StringBuilderBuffer( const StringBuilderBuffer& ) = delete;

            StringBuilderBuffer( StringBuilderBuffer&& ) = delete;

            StringBuilderBuffer& operator=( const StringBuilderBuffer& ) = delete;

            StringBuilderBuffer& operator=( StringBuilderBuffer&& ) = delete;

        protected:
            std::streamsize xsputn( const Char* s, std::streamsize count ) override {
                if ( count > this->epptr() - this->pptr()) count = this->epptr() - this->pptr();
                StringView stringView( s, static_cast<typename StringView::size_type>(count));
                string += stringView;
                this->pbump( static_cast<int>(count));
                return count;
            }

        private:
            String& string;
        };
    }

    template< typename Char = char,
              typename Traits = std::char_traits<Char>,
              typename Allocator = std::allocator<Char> >
    class StringBuilder {
    public:
        using String  = std::basic_string<Char, Traits, Allocator>;
    private:
        using Buffer  = detail::StringBuilderBuffer<Char, Traits, Allocator>;
        using Ostream = std::basic_ostream<Char, Traits>;
    public:
        explicit StringBuilder( typename String::size_type n ) : string( detail::string<Char, Traits, Allocator>( n )),
                                                                 buffer( std::get<String>( string )),
                                                                 ostream( &buffer ) {}

        explicit StringBuilder( String& string ) : string( &string ),
                                                   buffer( *( std::get<String*>( this->string ))),
                                                   ostream( &buffer ) {}

        StringBuilder( const StringBuilder& ) = delete;

        StringBuilder( StringBuilder&& ) = delete;

        StringBuilder& operator=( const StringBuilder& ) = delete;

        StringBuilder& operator=( StringBuilder&& ) = delete;

        operator String()&& { // NOLINT(google-explicit-constructor)
            return static_cast<StringBuilder&&>(*this).toString();
        }

        template< typename T >
        friend StringBuilder&& operator<<( StringBuilder&& stringBuilder, const T& t ) {
            stringBuilder.ostream << t;
            return std::move( stringBuilder );
        }

        template< typename Char_, typename Traits_ >
        friend std::basic_ostream<Char_, Traits_>&
        operator<<( std::basic_ostream<Char_, Traits_>& out, StringBuilder&& stringBuilder ) {
            return out << std::move( stringBuilder ).toString();
        }

    private:
        String toString()&& {
            return std::holds_alternative<String>( string ) ?
                   std::move( std::get<String>( string )) :
                   *( std::get<String*>( string ));
        }

        std::variant<String*, String> string;
        Buffer                        buffer;
        Ostream                       ostream;
    };
}
