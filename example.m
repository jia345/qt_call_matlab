% Simple MATLAB calculation example
feature('DefaultCharacterSet', 'UTF8');
disp("MATLAB例子开始...");

% Simple numeric calculations
a = 10;
b = 5;

% Addition
sum_result = a + b;
disp(['Addition: ', num2str(a), ' + ', num2str(b), ' = ', num2str(sum_result)]);

% Subtraction
diff_result = a - b;
disp(['Subtraction: ', num2str(a), ' - ', num2str(b), ' = ', num2str(diff_result)]);

% Multiplication
prod_result = a * b;
disp(['Multiplication: ', num2str(a), ' * ', num2str(b), ' = ', num2str(prod_result)]);

% Division
div_result = a / b;
disp(['Division: ', num2str(a), ' / ', num2str(b), ' = ', num2str(div_result)]);

% Square root
sqrt_result = sqrt(a);
disp(['Square root: sqrt(', num2str(a), ') = ', num2str(sqrt_result)]);

% Power operation
power_result = a^b;
disp(['Power: ', num2str(a), '^', num2str(b), ' = ', num2str(power_result)]);

% Create simple vector and calculate sum
vector = [1, 2, 3, 4, 5];
vector_sum = sum(vector);
disp(['Sum of vector [1,2,3,4,5] = ', num2str(vector_sum)]);

disp('MATLAB calculation example completed.'); 