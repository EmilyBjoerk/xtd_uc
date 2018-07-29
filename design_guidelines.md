# Writing hardware abstractions
When writing abstractions over hardware that can use interrupts a few things must be considered.

## Synchronous VS Asynchronous
Consider an serial communication interface. A synchronous interface is easy to work with but prevents
the MCU from doing useful work while waiting for the communications hardware to do it's thing. An asynchronous
interface avoids that limitation but instead introduces the need for a state machine to keep track of where
in a specific communication protocol one currently is. A fully asynchronous interface also requires callbacks
to be provided for the asynchronous events which require RAM to be stored in variables (not sure if the compiler
can eliminate the variables fully). Asynchronous design also often needs some kind of buffers which also requires
more RAM.

## Classes VS Free functions
Hardware is by definition singleton because there exists only one instance of that particular hardware and
it's registers. However using free functions means that it is harder to provide fakes for unit tests. One
can ofcourse introduce additional layers at the cost of more complexity. Using free functions is of course
easier on the implementation. In particular if the implementation requires the use of ISRs, finding the class
instance to call into from the ISR becomes an issue if you have a class based design. Classes do provide nice
encapsulation and the option of configuring the class with template parameters.

## How do deal with ISRs?
We want to try to avoid busy loops where possible and instead rely on hardware IRQs to act when needed.
An ISR is a free function which if we use a class based design needs a class instance to call into. 

## Configuration, template parameters, defines or constants?
There are different types of configuration that we need to address:
 * Configuration to adapt to the hardware (CPU frequency, available EEPROM space etc)
 * Configuration of application (USB descriptors for example)

The former should be done using defines in the Makefile, the latter should be in the source code.
There exists a grayzone for configuration of communications interfaces (baud rate, parity bits etc).

## Recommendations
The general theme here is, leave the decision of using memory and buffers to the user (don't pay for what you
don't use) and try to stay flexible.

We will prefer to write the implementation using classes to model the hardware units this will improve testability
of the user code. The problem with how to get a class instance to call into from ISRs is solved by letting the user
define the ISR and call into the instance they have created. This also solves the issue with users wanting to hook
the IRQ for triggering some code. However it involves a non-obvious step which needs to be documented clearly.

Writing a fully asynchronous interface will require callbacks which can be done using templates to avoid the RAM
overhead. However the necessary state machine to deal with protocols is undesirable. Luckily the above design choice
allows the user to chose wheter they want asynchronous or not. By hooking the relevant IRQs they can call their own
asynchronous methods or simply use a synchronous flow and not hook the IRQs. This also neatly avoids the extra
memory need for specifying callbacks.

As for configuration we will use defines in the Makefiles for anything that adapts to the hardware level differences
and use configuration in code using template parameters or constexprs where it makes sense for application configuration.
When unsure, lean towards using defines.