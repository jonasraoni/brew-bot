# Brew Bot

Several devices didn't implement the Qualcomm BREW platform as it should be. Some APIs when called could even crash or restart such devices.

The project aimed to find such problematic APIs and decrease the massive amount of time spent on testing applications on each new device that was released into the market.

The project is composed of two applications:

- [Matcher](matcher): Runs in the desktop, parses all the C/C++ code of the application which is going to be tested, extracts which BREW APIs were used and generates an output file (which will then be used as input of the testing application).

- [Tester](tester): Runs in the mobile device, tests each API used by the application and generates a report file on the phone. In the case the application/device dies, the tests can be resumed. The application contains in itself simple tests for all the BREW APIs, which were created upon demand.


## How to use
1. Extract the BREW APIs from the C headers by using the [ctags](https://github.com/universal-ctags/ctags) project. A text output like the snippet below will be generated, save it to a file.  
<code>
IImage	c:\brew\inc\AEE.h	263;"	t  
AEECallback	c:\brew\inc\AEE.h	281;"	t  
EVT_APP_START	c:\brew\inc\AEE.h	346;"	d  
EVT_APP_STOP	c:\brew\inc\AEE.h	347;"	d  
EVT_APP_SUSPEND	c:\brew\inc\AEE.h	348;"	d  
AECHAR	c:\brew\inc\AEE.h	75;"	t
</code>

2. Call the [matcher](matcher) application passing as first argument the ctags, the next arguments must be source code paths of your application, wildcards like *.c are accepted. It will generate a text file containing all the APIs that are being used by your application.

3. Run the tester [tester](tester) on the device.
