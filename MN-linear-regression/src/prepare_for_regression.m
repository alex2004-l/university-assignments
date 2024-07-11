function [FeatureMatrix] = prepare_for_regression(InitialMatrix)
  % InitialMatrix -> the matrix that must be transformed

  % FeatureMatrix -> the matrix with all training examples
  
  [m n] = size(InitialMatrix);
  
  for i = 1:m
    % indica deplasarea coloanelor
    depl = 0;
    for j = 1:n
      if strcmp(InitialMatrix{i,j}, "yes")
        FeatureMatrix(i,j+depl) = 1;
      elseif strcmp(InitialMatrix{i,j}, "no")
        FeatureMatrix(i,j+depl) = 0;
      % pentru furnished/unfurnished/semi-furnished se tranforma in perechea indicata
      % si se incrementeaza factorul de deplasare al coloanelor
      elseif strcmp(InitialMatrix{i,j}, "furnished")
        FeatureMatrix(i,j+depl) = 0;
        FeatureMatrix(i,j+1+depl) = 0;
        depl ++;
      elseif strcmp(InitialMatrix{i,j}, "unfurnished")
        FeatureMatrix(i,j+depl) = 0;
        FeatureMatrix(i,j+1+depl) = 1;
        depl ++;
      elseif strcmp(InitialMatrix{i,j}, "semi-furnished")
        FeatureMatrix(i,j+depl) = 1;
        FeatureMatrix(i,j+1+depl) = 0;
        depl ++;
      else
        % se foloseste str2double pentru ca toate elementele au fost salvate ca string-uri
        FeatureMatrix(i,j+depl) = str2double(InitialMatrix{i,j});
      endif
    endfor
  endfor

endfunction