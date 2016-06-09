# LibTelegram #
Fast, efficient, header-only C++ Telegram bot API library using FastCGI.

This Telegram library is for anyone who is building serious Telegram bots where
performance, scalability and maintainability matter.  Whether you're writing a
bot to do heavy lifting, servicing thousands of users concurrently through a
single webserver, or you just want to write your bot using clean, safe modern
C++11/14/17, this is your framework.

# Features #
* Modern, paradigmatic, clean and professional C++14 / C++17 programming style.
* Header-only - nothing to compile.
* Completely permissive MIT license - no restrictions on what you can do with the library.
* Minimum dependencies - the only external requirement is the ubiquitously available boost_system.
* Modular design - receiving and sending are totally separate, only include the components you use.
* Callback-based architecture means you retain full control of program flow and objects.
* Highly efficient asynchronous, multithreaded architecture allows a single instance to service multiple simultaneous requests at different rates without slowdown or queuing.
* Support for handling any number of bots concurrently from a single instance - instantiate any number of separate standalone listeners and senders.
* Persistent operation - with fastcgi, the bot keeps running between multiple requests, unlike slow cgi where a new instance has to spawn for each request.
* Persistence means it's also possible to preserve state between calls without resorting to writing to disk or a database!
* Efficient fastcgi protocol - data is transferred from the webserver by a binary format for minimal cpu overhead in repeat encoding and decoding.
* No wasteful polling - cgi architecture means the bot is active only when a message from telegram is received, and your callback functions are called instantly rather than after a poll interval.
* No need to run on its own port, works with your existing fastcgi-capable webserver.
* Each command is optimised and tailored to only send non-default parameters, even when the default is manually specified, to save every little bit of bandwidth.
* Both high and low level callbacks and sending functions available - send messages with a single short command, or craft and decode your own custom json however you like.
* Friendly high-level interface - clean, minimal and easy to use for the most common tasks.
* Low level functionality exposed - you can peek at the headers in Telegram's web requests if you like, or craft raw data to send to telegram by hand if you really want to.
* Detailed real time state monitoring data and lifetime statistics to help you diagnose bottlenecks and squeeze every last drop of performance from your code.

# Dependencies #
* Boost 1.60 upwards
* Boost CGI (included)
* URDL (included)

# How to use #
The library itself is header-only so there is nothing to build.  Simply
`#include "libtelegram/libtelegram.h"` and off you go.

## Setting the webhook ##
A bash script is provided to make it convenient to set the web hook for your bot.
Simply run:
`./set_webhook.sh 229029247:AAHfNUvcGS_ttvvulZG3Qi4NmMrjebH8F6w "https://mywebsite.com/fcgi-bin/my_fastcgi_webhook"`
(obviously substituting your own token and website).

You can also set the webhook yourself by just sending an appropriate request
from your browser, check the telegram bot documentation for the correct syntax.

It's possible to run multiple bots with a single program, and multiple webhooks -
see below under "advanced usage".

## Refer to the examples ##
Consult with the [examples](https://github.com/slowriot/libtelegram/blob/master/examples/)
for further instruction - this framework aims to be as easy to use as possible,
so the examples should be self-documenting.

### How to build the examples ###
Each example can be built with the included `Makefile`, by using
`make TARGET=example_echo -j4` where `echo` is the example and `4` is the number
of threads to use to build.  Alternatively, just use them as the starting point
of your own project with your own build system.

# Crossplatform Compatibility #
All components used in this library should be fully cross-platform compatible;
that means at the very least Linux, OS X and Windows.  However, it has not been
tested anywhere but Linux yet, and the examples and their Makefile may require
modificaiton to build on other platforms.  Let's face it, though, real servers
are always going to be Linux.

# Why not use one of the other C++ telegram bot frameworks? #
Have a look at their code for yourself.  The primary alternatives to this project
are written in a messy java-like style, and littered with `using namespace std`.
The code is full of unnecessary copies and inefficiencies, and fundamental
features such as SSL support are often missing.

* Here is a simple echo example from the leading alternative project: [samples/echobot/src/main.cpp](https://github.com/reo7sp/tgbot-cpp/blob/master/samples/echobot/src/main.cpp)
* And here is the same functionality with LibTelegram: [examples/echo.cpp](https://github.com/slowriot/libtelegram/blob/master/examples/echo.cpp)

You decide in which style you would prefer to write your next bot!

# Advanced usage #
#### One-way bots and partial usage ####
There's no requirement to use both the sender and receiver components.  If you'd
like to make a bot that only sends, then you don't need to use the listener in
your program, and it can run standalone rather than through your webserver.

In this case, you may wish to only `#include "libtelegram/sender.h"`.

Likewise, if you want to have a bot that only receives but never sends - perhaps
for a logging or statistics system - you can make do with the listener, and no
senders.  In that case, you can just `#include "libtelegram/listener.h"`.

#### Managing multiple bots ####
To send from multiple bots simultaneously, simply create multiple senders, each
initiated with the API key of the bot you wish to send as - it's that simple.

To receive from multiple bots, just use a single listener and symlink your bot
to multiple webhook locations on your web server, then distinguish between the
bots by examining the URI each callback is called for.

#### Maintaining state without a database or writing to disk ####
If your bot has short-term state you'd like to maintain between requests, you
can rely on your fastcgi implementation to keep a single instance alive between
requests in order to do so.

However, be aware that you should write anything to disk or DB that it would be
a problem to lose - for instance if the webserver undergoes a routine restart,
or it decides to cull your fastcgi processes due to a timeout.

If you want to rely on this system for maintaining state in a conversation
with a single user, you may need to adjust your fastcgi server's settings to
only allow a single concurrent instance - otherwise you may find your webserver
runs two or more instances of your bot, and the same user may be serviced by
two different instances on subsequent requests; the second instance would of
course have no memory of servicing that user before, even if the instance was
already alive at the time.

#### Turning off SSL ####
Please don't do this, for the sake of your users.  The costs of ssl are minimal,
and passing your users' communications in the open is simply barbaric.  But this
is your bot, so if you really feel you must turn off SSL, then define `LIBTELEGRAM_DISABLE_SSL_NO_REALLY_I_MEAN_IT_AND_I_KNOW_WHAT_IM_DOING`
before you include LibTelegram's headers.  But don't say we didn't warn you.

# Contributions #
This project is an early work in progress, and contributions are always welcome.
The following areas especially require attention at present:

* Tests - if you'd like to write tests for any components and set up test coverage for them, it would be very helpful.
* Additional convenience functions - any parts of the Telegram API that aren't covered by their own sending functions or callbacks can be added.

## Code Quality ##
Contributors are asked to adhere to the [VoxelStorm Style Guide](https://bitbucket.org/voxelstorm/voxelstorm/wiki/Style%20Guide) -
code not adhering to the style guide may be accepted, but will be converted to
match the style guide.
