# LibTelegram #
Fast, efficient, header-only C++ Telegram bot API library using FastCGI.

This Telegram library is for anyone who is building serious Telegram bots where
performance, scalability and maintainability matter.  Whether you're writing a
bot to do heavy lifting, servicing thousands of users concurrently through a
single webserver, or you just want to write your bot using clean, safe modern
C++20, this is your framework.

# What does it do? #
LibTelegram handles the business of communicating with Telegram for you.  It
consists of two major components - a listener system, and a sender system.  The
two are completely decoupled, so you can use both together in a single program,
or just one or the other.

The listeners are fully modular - you can choose which listener to use for any
program.  With all listeners, you set callbacks for those events you're
interested in being notified about, and they fire when needed.  All listeners
automatically handle acknowledgement replies to Telegram, dealing with errors
and decoding the incoming json for you.

### Polling Listener ###
This listener connects to the Telegram API and requests updates.  It uses long
polling, leaving a single connection open for as long as possible, so it
receives events immediately without delay, and without hammering the Telegram
API server with repeated connections.  This listener runs single-threaded and
calls your callbacks sequentially, in the order of the events received from
Telegram.  This is a good fit for a standalone bot where persistent operation
is required, and a moderately high level of traffic is expected (one or more
interactions per minute at all times of day).

### FastCGI Listener ###

** NOTE: FastCGI is disabled in the current version, due to the underlying library becoming outdated vs ASIO.  It only with Boost 1.69 and earlier - see https://github.com/darrengarvey/cgi/issues/16.  Contributors are welcome to migrate to an alternative fastcgi library.  Meanwhile, the polling listener works, and is now the default. **

This listener speaks fastcgi and is designed to be launched by your webserver.
It runs multithreaded.  You set callbacks, and it asynchronously listens for
events from Telegram and calls them when needed.  The listener takes care of
talking to your webserver and decoding the fastcgi requests.
Fastcgi means a single listener can be persistent across many connections, and
can handle many connections simultaneously, unlike traditional cgi.  Using a
webserver based system means you can have hundreds of separate bots running on a
single machine, rather than being limited to one per port as with bots that run
their own webserver.  This listener is a good fit for when you are hosting a
large number of bots which spend most of their time idle - the bots will only
run when required, and no bandwidth is wasted in establishing polling
connections.  This is also a good fit for some very traffic-heavy bots that
process large amounts of data, such as photographs and file uploads, as it can
service many connections from Telegram simultaneously without blocking.

### Sender ###
The sender provides a set of convenience functions for sending correctly
formatted messages to Telegram - it handles forming the correct data structures,
encoding as json, making the web request with SSL, dealing with timeouts,
handling and reporting errors, and finally returning to you the results of your
query.

All Telegram types are encapsulated in type-safe objects, and elegant error
tolerance is provided in a monadic way using std::optional.  As well as using
the native types, you have the option to access any values decoded from incoming
json, and to write the outgoing json yourself, either raw or in convenient json
object format.

# Features #
* Modern, paradigmatic, clean and professional C++20 programming style.
* Header-only - nothing to compile.
* Completely permissive MIT license - no restrictions on what you can do with the library.
* Minimum dependencies - the only external requirement is the ubiquitously available boost_system.
* Modular design - receiving and sending are totally separate, only include the components you use.
* Callback-based architecture means you retain full control of program flow and objects.
* Highly efficient asynchronous, multithreaded architecture allows a single instance to service multiple simultaneous requests at different rates without slowdown or queuing.
* Support for handling any number of bots concurrently from a single instance - instantiate any number of separate standalone listeners and senders.
* Persistent operation - with both the fastcgi and polling listener, the bot keeps running between multiple requests, unlike slow cgi where a new instance has to spawn for each request.
* Persistence means it's also possible to preserve state between calls without resorting to writing to disk or a database!
* Efficient fastcgi protocol - data is transferred from the webserver by a binary format for minimal cpu overhead in repeat encoding and decoding.
* <del>No wasteful polling - cgi architecture means the bot is active only when a message from telegram is received, and your callback functions are called instantly rather than after a poll interval.</del> (see above re. FastCGI])
* No need to run on its own port, works with your existing fastcgi-capable webserver.
* Each command is optimised and tailored to only send non-default parameters, even when the default is manually specified, to save every little bit of bandwidth.
* Both high and low level callbacks and sending functions available - send messages with a single short command, or craft and decode your own custom json however you like.
* Friendly high-level interface - clean, minimal and easy to use for the most common tasks.
* Low level functionality exposed - you can peek at the headers in Telegram's web requests if you like, or craft raw data to send to telegram by hand if you really want to.
* Detailed real time state monitoring data and lifetime statistics to help you diagnose bottlenecks and squeeze every last drop of performance from your code.

# Dependencies #
* C++20-compatible compiler with std::optional library support (GCC/G++ 7 upwards)
* Boost 1.76 upwards - http://www.boost.org
* <del>Boost CGI - https://github.com/slowriot/cgi (included, only needed for CGI / FastCGI listener)</del> (see above re. FastCGI])
* URDL - https://github.com/chriskohlhoff/urdl (included)
* JSON for Modern C++ - https://nlohmann.github.io/json/ (included)
* cpp-httplib - https://github.com/yhirose/cpp-httplib (included)

# Project status #
The project is currently in an intermediate stage, so some native convenience
functions and types are missing or incomplete - however, almost all
functionality is available by reading or specifying the json you require
manually through the functions that expose the json types to you.

## Not yet supported ##

* Stickers
* Uploading files directly
* Message entities
* Games
* Native functions to export chat links and set chat pictures

## Supported ##
All Telegram API types and methods not mentioned above.

# How to use #
The library itself is header-only so there is nothing to build.  Simply
`#include "libtelegram/libtelegram.h"` and off you go.

## Setting the webhook ##
A bash script is provided to make it convenient to set the web hook for your bot.  This is only required if using FastCGI.
Simply run:
`./set_webhook.sh 229029247:AAHfNUvcGS_ttvvulZG3Qi4NmMrjebH8F6w "https://mywebsite.com/fcgi-bin/my_fastcgi_webhook"`
(obviously substituting your own token and website).

You can also set the webhook yourself by just sending an appropriate request
from your browser, check the telegram bot documentation for the correct syntax.

It's possible to run multiple bots with a single program, and multiple webhooks
or polling listeners - see below under "advanced usage".

## Refer to the examples ##
Consult with the [examples](https://github.com/slowriot/libtelegram/blob/master/examples/)
for further instruction - this framework aims to be as easy to use as possible,
so the examples should be self-documenting.

Simpler examples spend more time explaining simple concepts in comments, while
the more advanced examples save the comments for the more advanced features.
For that reason, there's a suggested reading order for the examples:

1. Minimal echo: [examples/echo.cpp](https://github.com/slowriot/libtelegram/blob/master/examples/echo.cpp)
1. Echo using raw json access: [examples/echo_json.cpp](https://github.com/slowriot/libtelegram/blob/master/examples/echo_json.cpp)
1. Fetch a file from the web and read it back: [examples/webfetch.cpp](https://github.com/slowriot/libtelegram/blob/master/examples/webfetch.cpp)
1. Get and process photos sent by the user: [examples/getphoto.cpp](https://github.com/slowriot/libtelegram/blob/master/examples/getphoto.cpp)
1. Stateful calculator, using a custom keyboard: [examples/calculator.cpp](https://github.com/slowriot/libtelegram/blob/master/examples/calculator.cpp)
1. Accept locations and update a live location: [examples/livelocation.cpp](https://github.com/slowriot/libtelegram/blob/master/examples/livelocation.cpp)

## How to build the examples ##
### CMake
All examples can be built with CMake, by doing the following:
```
mkdir build
cd build
cmake ..
make -j$(nproc)
```

### Others
Also included are project files for the Code::Blocks IDE - just open the
libtelegram.workspace file in Code::Blocks and you can build all of the examples.

Alternatively, just use the examples as the starting point of your own project,
building it with your own build system.

#### Running the examples ####
The default examples use polling, so simply execute them in the terminal.

### Setting up FastCGI ###
** Note - FastCGI is currently non-functional - see above **

There are a number of fastcgi systems, and it will depend largely on your
webserver.  Below are some example instructions for how to set up fastcgi on
Apache2:

* Install Apache2, obviously.
* Get module `fastcgi`.  On debian and noobuntu you can
`apt-get install libapache2-mod-fastcgi`.
* Use the following sort of configuration to enable a directory to execute
fastcgi scripts:
```
  ScriptAlias /fcgi-bin/ /var/www/mysite/fcgi-bin/
  <Directory /var/www/mysite/fcgi-bin/>
    Options +ExecCGI
    SetHandler fastcgi-script
  </Directory>
```
* Then simply place your executable binaries in this directory, or
subdirectories, taking care to give them executable permissions.
* One caveat is that module `fastcgi` does not currently seem to work with
`mpm_itk_module` running as any user other than `www-data` on Debian.
* Something else to watch out for when testing is that unlike normal cgi,
the program persists and keeps running - even after you overwrite the binary.
Make sure you either manually kill your bot when you want to update to a new
version, or restart the Apache server, or build in functionality for it to exit
by itself on command.  The webserver will then bring it back up on demand.
* Anything you send to `std::cerr` will appear in `/var/log/apache2/error.log` -
even if you have vhosts set up with custom logging, the fastcgi module won't
send to those.  Don't try to output anything to `std::cout`.

# Crossplatform Compatibility #
All components used in this library should be fully cross-platform compatible;
that means at the very least Linux, OS X and Windows.  However, it has not been
tested anywhere but Linux yet, and the examples and the CMakeLists.txt may require
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
### Switching between polling and CGI ###
In order to use CGI, you must set a webhook; a helper script, `set_webhook.sh`
is provided for your convenience.  It takes two arguments - the API key, and the
full https URL of your endpoint.  However, if switching back to polling, don't
forget to unset the webhook or you won't receive polling updates!  To unset the
webhook, just call the script with no second argument.

### One-way bots and partial usage ###
There's no requirement to use both the sender and receiver components.  If you'd
like to make a bot that only sends, then you don't need to use the listener in
your program, and it can run standalone rather than through your webserver.

In this case, you may wish to only `#include "libtelegram/sender.h"`.

Likewise, if you want to have a bot that only receives but never sends - perhaps
for a logging or statistics system - you don't necessarily need to use a sender.
If you are using one of the CGI listeners, then no sender is needed. However,
the polling listener still needs a sender which it uses to poll Telegram for
events.

### Managing multiple bots ###
To send from multiple bots simultaneously, simply create multiple senders, each
initiated with the API key of the bot you wish to send as - it's that simple.

To receive from multiple bots with FastCGI or CGI, just use a single listener
and symlink your bot to multiple webhook locations on your web server, then
distinguish between the bots by examining the URI each callback is called for.

To receive from multiple bots with polling, create a thread for each polling
listener to run in.  They can each either have their own sender, or can share
a single global sender.  Senders maintain no global state and create independent
connections to the server for each request, thus ensuring lock-free and
wait-free thread safety.  Calling methods concurrently on the same sender from
multiple threads is therefore perfectly safe, should you choose to do that.  On
the other hand, each sender also has a tiny memory footprint, so creating many
senders has very little cost.

### Maintaining state without a database or writing to disk with FastCGI ###
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

If you're running the polling listener, your program will run as long as there
are no fatal errors with the listener, and you can get away with saving state
after `listener.run()` has completed.  However, it would be wise to also save
your state periodically in case your bot crashes or is killed by something.

### Turning off SSL ###
Please don't do this, for the sake of your users.  The costs of ssl are minimal,
and passing your users' communications in the open is simply barbaric.  But this
is your bot, so if you really feel you must turn off SSL, then define
`LIBTELEGRAM_DISABLE_SSL_NO_REALLY_I_MEAN_IT_AND_I_KNOW_WHAT_IM_DOING`
before you include LibTelegram's headers.  But don't say we didn't warn you.

### Signal handlers ###
The polling listener (and possibly other future components) sets a signal
handler to catch ctrl-c and shut down neatly, after the next poll.  This is
currently the only elegant way of shutting down listener::run() for the polling
listener; it is assumed that in a CGI or FCGI program, the webserver will
terminate its children immediately whenever it wants, so there is no attempt to
handle signals in other listeners.  The listener only sets the signal handler
at the beginning of run() and unsets it at the end.

You can also manually set or release this signal handler at any time using the
`telegram::listener::poll::set_signal_handler()` and `unset_signal_handler()`
functions, and check its current state with `is_signal_handler_set()`.

However, if you want to set your own signal handlers, and don't want this to
interrupt your global state, then define `TELEGRAM_NO_SIGNAL_HANDLER` in your
program.  In this case, be aware that run() will continue indefinitely until you
tell it to stop (from another thread); to do this you can call the
`telegram::listener::poll::stop()` function.  You can also call `stop_all()` to
queue a global halt for all poll listeners that exist in your program (which is
what the signal handler does).

Windows users: The default signal handler requires `sigaction` and will not
compile on Windows; as a result, TELEGRAM_NO_SIGNAL_HANDLER is defined by
default for Windows builds.

### Disabling specific components ###
You can exclude individual modular components you're not using, which may remove
compile time dependencies on certain libraries.  Specific parts can be
disabled at compile time with:

* `#define TELEGRAM_NO_LISTENER_FCGI`
* `#define TELEGRAM_NO_LISTENER_POLL`

# Contributions #
This project is an early work in progress, and contributions are always welcome.
The following areas especially require attention at present:

* Tests - if you'd like to write tests for any components and set up test coverage for them, it would be very helpful.
* Additional convenience functions - any parts of the Telegram API that aren't covered by their own sending functions or callbacks can be added.

## Code Quality ##
Contributors are asked to adhere to the [VoxelStorm Style Guide](https://bitbucket.org/voxelstorm/voxelstorm/wiki/Style%20Guide) -
code not adhering to the style guide may be accepted, but will be converted to
match the style guide.

## Projects using LibTelegram ##
This is not an exhaustive list; if you are building a project using this
library, please contribute your project to this list.
* https://github.com/slowriot/telegramtrackerbot
* https://code.voxelstorm.com/photocontestbot
* https://github.com/Shadouw/tbSheena
