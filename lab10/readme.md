# ChatGPT assisted code reviews

This is a sample session with file content from upstream repository. You should repeat the procedure for each major C source file in your repository. As a result, there should be clear evidence of gitlab issues (and closed issues with linked commits).  

Example:  

1. Log in to ChatGPT and start a new conversation
2. Ask ChatGPT to do a code review on (paste full source code):

```
do code review on this: "/*
 * event_test_with_threads.c
 *
 *  Created on: Feb 14, 2022
 *      Author: jarno
 */


#if 1

#include <gpiod.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

...code cut for readability, used full source file content in reality

end:
	return ret;
}
#endif
"
```
3. For each point in review, create a gitlab issue (and assign it to yourself)
```
Here's a code review of the provided code:

Naming Conventions:
The naming conventions used in the code are inconsistent. Some variable names use camelCase while others use snake_case. This can make the code harder to read and understand. Consider using a consistent naming convention throughout the code.

1. Magic Numbers:
The GPIO pin numbers (22, 23, 24, 25) are defined as constants, which is a good practice. However, it is recommended to use meaningful names for these constants instead of just numbers, to make the code easier to understand. For example, instead of BUTTON_PIN, you could use BUTTON_GPIO_PIN.

2. Use of true in while loop:
In C, the preferred way to represent a boolean value is to use the bool data type, which is defined in stdbool.h. The use of true in the while loop should be replaced with true from stdbool.h or a #define of true as 1.

3. Return value of gpio_function:
The return type of gpio_function is void *, but the function returns (void *)(ret) where ret is an int. When returning a pointer from a function, it's good practice to return NULL instead of a pointer to a negative integer value, as a negative integer value may be a valid address.

4. Error Handling:
The code only has basic error handling, using the perror function. In a production-level code, it's important to have proper error handling and logging, to allow debugging and fixing issues in case of unexpected behavior. Consider adding error handling and logging for important operations in the code.

5. Documentation:
The documentation provided at the top of the code is limited, making it harder for someone else to understand the purpose and behavior of the code. Consider adding more detailed documentation that includes an overview of the code, its purpose, and how it works.

6. Use of Global Variables:
The code uses a few global variables, such as chip, lineButton, lineRed, lineYellow, and lineGreen. It's generally better to avoid using global variables, as they can lead to unexpected behavior when multiple threads or functions access the same global variable simultaneously. Consider using local variables, or passing variables as parameters to functions where necessary.
```

4. Solve all issues by editing your code accordingly, and in commit message add reference to issue number as explained in https://docs.gitlab.com/ee/user/project/issues/managing_issues.html#closing-issues-automatically 
