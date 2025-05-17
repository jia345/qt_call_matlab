function result = calculate_with_parameters(a, b)
% CALCULATE_WITH_PARAMETERS Calculate and display various operations
%   This function performs various mathematical operations on the inputs
%   and displays the results.
%
%   Parameters:
%       a - First number
%       b - Second number
%
%   Returns:
%       result - The sum of a+b and a*b

    fprintf('Parameters received: a = %d, b = %d\n', a, b);
    
    % Perform calculations
    sum_result = a + b;
    fprintf('Addition: %d + %d = %d\n', a, b, sum_result);
    
    diff_result = a - b;
    fprintf('Subtraction: %d - %d = %d\n', a, b, diff_result);
    
    product_result = a * b;
    fprintf('Multiplication: %d * %d = %d\n', a, b, product_result);
    
    if b ~= 0
        div_result = a / b;
        fprintf('Division: %d / %d = %.2f\n', a, b, div_result);
    else
        fprintf('Division: Cannot divide by zero\n');
    end
    
    % Return result
    result = sum_result + product_result;
    fprintf('Final result: %d + %d + %d * %d = %d\n', a, b, a, b, result);
end 