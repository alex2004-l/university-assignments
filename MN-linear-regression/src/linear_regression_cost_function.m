function [Error] = linear_regression_cost_function(Theta, Y, FeatureMatrix)
  % Theta -> the vector of weights
  % Y -> the vector with all actual values
  % FeatureMatrix -> the matrix with all training examples

  % Error -> the error of the regularized cost function
  
  [m n]= size(FeatureMatrix);
  % se afla h sub forma de vector pentru fiecare set de antrenament
  h = FeatureMatrix * Theta(2:n+1);
  % se calculeaza eroare conform formulei
  Error = sum((h-Y).^2)/(2*m);
  
endfunction