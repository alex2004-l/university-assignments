function [Theta] = gradient_descent(FeatureMatrix, Y, n, m, alpha, iter)
  % FeatureMatrix -> the matrix with all training examples
  % Y -> the vector with all actual values
  % n -> the number of predictors
  % m -> the number of trainings
  % alpha -> the learning rate
  % iter -> the number of iterations

  % Theta -> the vector of weights

  % Theta este initializat ca vectorul nul
  Theta = zeros(n+1,1);
  while iter > 0
    iter--;
    h = FeatureMatrix * (Theta(2:n+1));
    % se calculeaza pentru fiecare element al lui Theta noua valoare
    for j = 2:n+1
      dJ = 1/m * sum((h-Y).*FeatureMatrix(:, j-1));
      Theta(j) = Theta(j) - alpha * dJ;
    endfor
  endwhile
  
endfunction