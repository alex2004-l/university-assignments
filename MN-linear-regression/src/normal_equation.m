function [Theta] = normal_equation(FeatureMatrix, Y, tol, iter)
  % FeatureMatrix -> the matrix with all training examples
  % Y -> the vector with all actual values
  % tol -> the learning rate
  % iter -> the number of iterations
  % tol -> the tolerance level for convergence of the conjugate gradient method

  % Theta -> the vector of weights

  [m n] = size(FeatureMatrix);
  % daca matricea nu are acelasi numar de linii si coloane se foloseste formula
  if m~= n
    Theta = inv(FeatureMatrix' * FeatureMatrix) * FeatureMatrix' * Y;
    Theta = [0; Theta];
  else
    %daca nu se foloseste metoda gradientului conjugat
    if all(eig(FeatureMatrix) > 0)
      r = v = Y;
      Theta0 = zeros(n,1);
      tol_squared = tol^2;
      k = 1;
      while k <= iter && r' * r > tol_squared
        t = (r' * r)/(v' * FeatureMatrix * v);
        Theta = Theta0 + t * v;
        r_new = r - t * FeatureMatrix * v;
        s = (r_new' * r_new) / (r' * r);
        v_new = r_new + s * v;
        % incrementare contor & actualizare valori pentru urmatoarea iteratie
        k = k + 1;
        Theta0 = Theta;
        r = r_new;
        v = v_new;
      endwhile
    endif
    Theta = [0; Theta0];
  endif

endfunction
