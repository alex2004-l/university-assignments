import scala.annotation.tailrec

object Regression {
  /* Regression function */
  def regression(dataset_file: String,
                attribute_columns: List[String],
                value_column: String,
                test_percentage: Double,
                alpha: Double,
                gradient_descent_steps: Int): (Matrix, Double) = {

    /* Auxiliary function using the gradient descendent algorithm for calculating the predictions */
    @tailrec
    def gradient_descendent(train_mat: Matrix, estimate_train: Matrix, W: Matrix, steps: Int): Matrix = {
      if (steps == 0) W
      else {
        val errors = train_mat * W - estimate_train
        val gradient = train_mat.height match {
          case Some(value) => {
            (train_mat.transpose * errors).map(x => x/value)
          }
          case None => new Matrix(None)
        }
        val new_W = W - gradient.map(_ * alpha)
        /* Recursive call of the auxiliary function */
        gradient_descendent(train_mat, estimate_train, new_W, steps - 1)
      }
    }

    /* Converting the file into a dataset */
    val dataset = Dataset.apply(dataset_file)

    /* Splitting the dataset comprised of the columns of interest */
    val (train, test) = dataset.selectColumns(attribute_columns :+ value_column).split(test_percentage)

    /* Separating the dataset into Matrix for train and test, respectively
     * into vectors with the value to estimate for test and train*/
    val m_train = Dataset.apply(train.data.map(line => line.dropRight(1)))
    val m_test = Dataset.apply(train.data.map(line => line.dropRight(1)))
    val (train_mat, test_mat) = (Matrix.apply(m_train), Matrix.apply(m_test))

    val e_train = Dataset.apply(train.data.map(line => line.takeRight(1)))
    val e_test = Dataset.apply(test.data.map(line => line.takeRight(1)))
    val (estimate_train, estimate_test) = (Matrix.apply(e_train), Matrix.apply(e_test))

    /* Adding an extra column */
    val new_train_mat = train_mat.++(1.0)
    val new_test_mat = test_mat ++ (1.0)

    /* Creating and populating W with zeros */
    val W = new_train_mat.width match {
      case Some(value) => new Matrix(Some(List.fill(value)(List.fill(1)(0.0))))
      case None => new Matrix(None)
    }

    /* Gradient function call */
    val W_final = gradient_descendent(new_train_mat, estimate_train, W, gradient_descent_steps)

    /* Calculating the error Matrix */
    val error_final = new_test_mat.height match{
      case Some(value) => ((new_test_mat * W_final) - estimate_test).map(_/value)
      case None => new Matrix(None)
    }

    /* Calculating the total error combined */
    val errf = error_final.data match {
      case Some(inner_list) => inner_list.foldLeft(0.0)((acc, line) => acc + line.head)
      case None => 0.0
    }

    /* Final results */
    (W_final, errf)
  }

  def main(args: Array[String]): Unit = {
    // Example
    print(regression("datasets/houseds.csv", List("GrLivArea", "YearBuilt"), "SalePrice", 0.1, 1e-7, 10000))
  }
}