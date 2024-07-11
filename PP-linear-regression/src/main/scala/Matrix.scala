type Mat = List[List[Double]]

class Matrix(m: Option[List[List[Double]]]) {

  /* Creates the transpose of a Matrix */
  def transpose: Matrix = {
    /* This function is inspired from Curs06 */
    def aux_transpose(mat: Mat): Mat = {
      mat match {
        case Nil :: _ => Nil
        case _ => mat.map(_.head) :: aux_transpose(mat.map(_.tail))
      }
    }

    /* Matching the inner option parameter, to see if the Matrix is valid */
    m match {
      /* In case it is valid, we create the transpose matrix using the auxiliary function */
      case Some(data) => new Matrix(Some(aux_transpose(data)))
      case None => this
    }
  }

  /* Maps each element x of the Matrix to f(x) */
  def map(f: Double => Double): Matrix = {
    /* Matching the inner option parameter, to see if the Matrix is valid */
    m match {
      case Some(data) => new Matrix(Some(data.map(_.map(x => f(x)))))
      case None => this
    }
  }

  /* Returns the result of the multiplication of this Matrix with the parameter Matrix */
  def *(other: Matrix): Matrix = {
    /* This function is inspired from Curs06 */
    def aux_mult(m1: Mat, m2: Mat): Mat = {
      m1.map(
        line => m2.map(
          col => line.zip(col).map(pair => pair._1 * pair._2).foldRight(0.0)(_ + _)))
    }

    /* The relevant dimensions for checking the validity of the multiplication.
    *  I used the getOrElse function instead of pattern matching for code readability.
    *  Note: I looked for a function similar with unwrap from Rust. */
    val width1 = this.width.getOrElse(0)
    val height2 = other.height.getOrElse(0)

    val otherData = other.data

    /* Pattern matching on the two inner data's of the two Matrix objects */
    (data, otherData) match {
      /* Cases in which the multiplication fails since at least one Matrix contains None */
      case (None, _) => this
      case (_, None) => new Matrix(None)
      /* Case in which both Matrix contain valid data */
      case (Some(d1), Some(_)) =>
        if (width1 == height2) {
          /* The outer match guarantees the transpose will be valid */
          val d2 = other.transpose.data match {
            case Some(value) => value
          }
          new Matrix(Some(aux_mult(d1, d2)))
        }
        else {
          /* Case in which the dimensions do not match */
          new Matrix(None)
        }
    }
  }

  /*  Creates a new Matrix which has an extra column populated with the value x */
  def ++(x: Double): Matrix = {
    data match {
      case Some(value) =>
        val newMat = value.map(lines => lines :+ x)
        new Matrix(Some(newMat))
      case None => this
    }
  }

  /* Returns the result of the subtraction of a Matrix from this one */
  def -(other: Matrix): Matrix = {
    /* Auxiliary function that performs the substraction on the inner Mat's */
    def aux_subtraction(m1: Mat, m2: Mat): Mat = {
      m1.zip(m2).map((line1, line2) => line1.zip(line2).map((e1, e2) => e1 - e2))
    }

    /* The relevant dimensions for checking the validity of the substraction. */
    val height1 = this.height.getOrElse(0)
    val width1 = this.width.getOrElse(0)
    val height2 = other.height.getOrElse(0)
    val width2 = other.width.getOrElse(0)

    val otherData = other.data

    /* Pattern matching on the two inner Option[Mat] of the two Matrix objects */
    (m, otherData) match {
      /* Cases in which the subtraction fails since at least one Matrix contains None */
      case (None, _) => this
      case (_, None) => new Matrix(None)
      /* Case in which both Matrix contain valid data */
      case (Some(d1), Some(d2)) =>
        if (height1 == height2 && width1 == width2) {
          /* Valid dimensions */
          new Matrix(Some(aux_subtraction(d1, d2)))
        } else {
          /* Dimensions do not match */
          new Matrix(None)
        }
    }
  }

  /* Functions for getting information related to the matrix */
  def data: Option[Mat] = m
  def height: Option[Int] = {
    m match {
      case Some(lines) => Some(lines.size)
      case None => None
    }
  }
  def width: Option[Int] = {
    m match {
      case Some(lines) => Some(lines.head.size)
      case None => None
    }
  }

  /* This function is inspired from Curs06 */
  override def toString: String = m
    .map(_.foldRight("")((x, y) => s"$x $y"))
    .foldRight("")(_ + "\n" + _)

}

object Matrix {
  /* Creates a Matrix from a Mat */
  def apply(data: Mat): Matrix = new Matrix(Some(data))

  /* Creates a Matrix from an Option[Mat] */
  def apply(data: Option[Mat]): Matrix = {
    data match {
      case Some(mat) => new Matrix(Some(mat))
      case None => new Matrix(None)
    }
  }

  /* Creates a Matrix from a Dataset */
  def apply(dataset: Dataset): Matrix = {
    /* Converts all dataset elements into double, or into None if toDouble fails */
    val data = dataset.getRows.map(_.map{e =>
      try {
        Some(e.toDouble)
      } catch {
        case _: Throwable => None
      }
    })

    /* Checks if the Matrix is invalid(if exists a None inside its data) */
    val invalidMatrix = data.exists(line => line.exists{
      case None => true
      case _ => false
    })

    /* Returns the new Matrix */
    if (invalidMatrix) new Matrix(None)
    else new Matrix(Some(data.map(_.map {
      case Some(l) => l
      case None => 0
    })))
  }
}
