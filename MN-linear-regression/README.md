## Price prediction using linear regression(Gradient descendent method) - MAY 2023
---

#### This project was implemented as part of the numerical methods coursework. The project involves predicting apartment prices based on various features using linear regression. Optimization algorithms are used to determine the model coefficients, specifically the gradient descent method and the normal equation.

---
- This project involves a series of tasks for reading, processing, and performing regression analysis on a dataset. Initially, data is read from a file.

- The next step involves preparing the data for regression. This includes encoding categorical string values and converting numeric values from strings to numbers. Special handling is required for terms like "unfurnished," "furnished," and "semi-furnished," which are encoded with specific values in the matrix.

- Data is also read from a CSV file, where the challenge was to store the data correctly in a cell matrix without duplicating cell specifiers. 

- Several regression cost functions were implemented, including linear, lasso, and ridge regression. These functions involve calculating the hypothesis vector for each training test and the regularization error.

- Gradient descent was implemented as described in the documentation. The initial approximation for the parameter vector was a zero vector, and subsequent values were calculated using the specified formulas. This method continues iterating until the specified number of iterations is reached.

- Finally, for matrices with mismatched dimensions, calculations were done using the first formula presented in the documentation.
For matrices with matching dimensions, the conjugate gradient method was applied, with iterative updates of the parameters to reflect transformations at each step.

---