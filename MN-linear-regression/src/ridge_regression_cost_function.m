function [Error] = ridge_regression_cost_function(Theta, Y, FeatureMatrix, lambda)
  % Theta -> the vector of weights
  % Y -> the vector with all actual values
  % FeatureMatrix -> the matrix with all training examples
  % lambda -> regularization parameter that controls the amount of 
  %           shrinkage applied to the regression coefficients

  % Error -> the error of the regularized cost function

  [m n]= size(FeatureMatrix);
  % se afla h sub forma de vector pentru fiecare set de antrenament
  h = FeatureMatrix * Theta(2:n+1);
  % se calculeaza eroare conform formulei
  Error = sum((h-Y).^2)/(2*m) + lambda * norm(Theta,2)^2;
  
endfunction