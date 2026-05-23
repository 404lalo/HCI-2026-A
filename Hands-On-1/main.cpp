//Hands-On 1 Systems simulation through prediction
#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>
#include <stdexcept>
#include <string>

class Matrix {
private:
    int rows_, cols_;
    std::vector<std::vector<double>> data_;
 
public:
    Matrix(int r, int c, double init = 0.0)
        : rows_(r), cols_(c), data_(r, std::vector<double>(c, init)) {}

    int rows() const { return rows_; }
    int cols() const { return cols_; }
    double& at(int i, int j)       { return data_[i][j]; }
    double  at(int i, int j) const { return data_[i][j]; }

    Matrix transpose() const {
        Matrix T(cols_, rows_);
        for (int i = 0; i < rows_; ++i)
            for (int j = 0; j < cols_; ++j)
                T.at(j, i) = data_[i][j];
        return T;
    }

    Matrix operator*(const Matrix& B) const {
        if (cols_ != B.rows_)
            throw std::invalid_argument("Dimensiones incompatibles para multiplicación.");
        Matrix C(rows_, B.cols_);
        for (int i = 0; i < rows_; ++i)
            for (int j = 0; j < B.cols_; ++j)
                for (int k = 0; k < cols_; ++k)
                    C.at(i, j) += data_[i][k] * B.at(k, j);
        return C;
    }

    Matrix inverse() const {
        if (rows_ != cols_)
            throw std::invalid_argument("Solo se puede invertir una matriz cuadrada.");
        int n = rows_;

        std::vector<std::vector<double>> aug(n, std::vector<double>(2 * n, 0.0));
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j)
                aug[i][j] = data_[i][j];
            aug[i][n + i] = 1.0;
        }

        for (int col = 0; col < n; ++col) {
            // Buscar fila pivote
            int pivot = col;
            for (int row = col + 1; row < n; ++row)
                if (std::abs(aug[row][col]) > std::abs(aug[pivot][col]))
                    pivot = row;
            std::swap(aug[col], aug[pivot]);
 
            double diag = aug[col][col];
            if (std::abs(diag) < 1e-12)
                throw std::runtime_error("Matriz singular o casi singular.");
 
            for (int j = 0; j < 2 * n; ++j)
                aug[col][j] /= diag;
 
            for (int row = 0; row < n; ++row) {
                if (row == col) continue;
                double factor = aug[row][col];
                for (int j = 0; j < 2 * n; ++j)
                    aug[row][j] -= factor * aug[col][j];
            }
        }

        Matrix inv(n, n);
        for (int i = 0; i < n; ++i)
            for (int j = 0; j < n; ++j)
                inv.at(i, j) = aug[i][n + j];
        return inv;
    }

    void print(const std::string& name = "") const {
        if (!name.empty()) std::cout << name << ":\n";
        for (int i = 0; i < rows_; ++i) {
            std::cout << "  [ ";
            for (int j = 0; j < cols_; ++j)
                std::cout << std::setw(12) << std::fixed << std::setprecision(4) << data_[i][j];
            std::cout << " ]\n";
        }
    }
};

class DataSet {
private:
    int n_;
    std::vector<double> x1_, x2_, y_;
 
public:
    DataSet() : n_(17) {

        x1_ = {41.9, 43.4, 43.9, 44.5, 47.3,
                47.5, 47.9, 50.2, 52.8, 53.2,
                56.7, 57.0, 63.5, 65.3, 71.1,
                77.0, 77.8};

        x2_ = {29.1, 29.3, 29.5, 29.7, 29.9,
                30.3, 30.5, 30.7, 30.8, 30.9,
                31.5, 31.7, 31.9, 32.0, 32.1,
                32.5, 32.9};

        y_  = {251.3, 251.3, 248.3, 267.5, 273.0,
                276.5, 270.3, 274.9, 285.0, 290.0,
                297.0, 302.5, 304.5, 309.3, 321.7,
                330.7, 349.0};
    }
 
    int size()      const { return n_; }
    double x1(int i) const { return x1_[i]; }
    double x2(int i) const { return x2_[i]; }
    double  y(int i) const { return  y_[i]; }

    Matrix designMatrix() const {
        Matrix X(n_, 3);
        for (int i = 0; i < n_; ++i) {
            X.at(i, 0) = 1.0;
            X.at(i, 1) = x1_[i];
            X.at(i, 2) = x2_[i];
        }
        return X;
    }

    Matrix yVector() const {
        Matrix yv(n_, 1);
        for (int i = 0; i < n_; ++i)
            yv.at(i, 0) = y_[i];
        return yv;
    }

    void printTable() const {
        std::cout << "\n";
        std::cout << std::setw(8)  << "Obs"
                  << std::setw(12) << "x1 (F1)"
                  << std::setw(12) << "x2 (F2)"
                  << std::setw(12) << "y (Yield)" << "\n";
        std::cout << std::string(44, '-') << "\n";
        for (int i = 0; i < n_; ++i) {
            std::cout << std::setw(8)  << i + 1
                      << std::setw(12) << std::fixed << std::setprecision(1) << x1_[i]
                      << std::setw(12) << x2_[i]
                      << std::setw(12) << y_[i] << "\n";
        }
    }
};

class PolynomialRegressor {
private:
    double b0_, b1_, b2_;
    bool   fitted_;
 
public:
    PolynomialRegressor() : b0_(0), b1_(0), b2_(0), fitted_(false) {}

    void fit(const DataSet& ds) {
        Matrix X  = ds.designMatrix();
        Matrix y  = ds.yVector();
        Matrix Xt = X.transpose();

        Matrix XtX     = Xt * X;
        Matrix XtX_inv = XtX.inverse();
        Matrix Xty     = Xt * y;
        Matrix beta    = XtX_inv * Xty;
 
        b0_ = beta.at(0, 0);
        b1_ = beta.at(1, 0);
        b2_ = beta.at(2, 0);
        fitted_ = true;

        std::cout << "\n=== Matrices del cálculo ===\n";
        X    .print("Matriz de diseño X (primeras 3 filas mostradas, tamaño 17×3)");
        XtX  .print("\nXᵀX");
        XtX_inv.print("\n(XᵀX)⁻¹");
    }

    double predict(double x1, double x2) const {
        if (!fitted_) throw std::runtime_error("Modelo no ajustado.");
        return b0_ + b1_ * x1 + b2_ * x2;
    }

    void printEquation() const {
        std::cout << "\n=== Ecuación de Regresión Lineal Múltiple ===\n";
        std::cout << "  ŷ = " << std::fixed << std::setprecision(6)
                  << b0_ << "  +  " << b1_ << " * x1"
                  << "  +  " << b2_ << " * x2\n";
    }

    void printParameters() const {
        std::cout << "\n=== Parámetros estimados (β̂) ===\n";
        std::cout << "  B0 = " << std::fixed << std::setprecision(6) << b0_ << "\n";
        std::cout << "  B1 = " << std::fixed << std::setprecision(6) << b1_ << "\n";
        std::cout << "  B2 = " << std::fixed << std::setprecision(6) << b2_ << "\n";
    }

    void verifyParameters(double ref_b0, double ref_b1, double ref_b2,
                          double tol = 1e-2) const {
        std::cout << "\n=== Verificación con valores de referencia ===\n";
        auto check = [&](const std::string& name, double calc, double ref) {
            double err = std::abs(calc - ref);
            bool ok = err < tol;
            std::cout << "  " << name
                      << "  calculado = " << std::setw(12) << std::fixed << std::setprecision(6) << calc
                      << "  referencia = " << std::setw(12) << ref
                      << "  |error| = "   << std::setw(10) << std::scientific << err
                      << "  " << (ok ? "✓ OK" : "✗ FALLA") << "\n";
        };
        check("B0", b0_, ref_b0);
        check("B1", b1_, ref_b1);
        check("B2", b2_, ref_b2);
    }

    void simulate(const std::vector<std::pair<double,double>>& experiments) const {
        std::cout << "\n=== Simulación: predicción de Yield ===\n";
        std::cout << std::setw(6)  << "Exp"
                  << std::setw(10) << "x1"
                  << std::setw(10) << "x2"
                  << std::setw(16) << "ŷ (yield)" << "\n";
        std::cout << std::string(42, '-') << "\n";
        int idx = 1;
        for (auto& [x1, x2] : experiments) {
            double yhat = predict(x1, x2);
            std::cout << std::setw(6)  << idx++
                      << std::setw(10) << std::fixed << std::setprecision(1) << x1
                      << std::setw(10) << x2
                      << std::setw(16) << std::setprecision(4) << yhat << "\n";
        }
    }
 
    double b0() const { return b0_; }
    double b1() const { return b1_; }
    double b2() const { return b2_; }
};

int main() {
    std::cout << "============================================================\n";
    std::cout << "  Regresión Lineal Múltiple – Mínimos Cuadrados (OOP C++)\n";
    std::cout << "  Modelo: ŷ = B0 + B1*x1 + B2*x2\n";
    std::cout << "  β̂ = (XᵀX)⁻¹ Xᵀy\n";
    std::cout << "============================================================\n";
 
    DataSet ds;
    std::cout << "\n=== Dataset: 17 Experimentos Químicos ===";
    ds.printTable();
    
    PolynomialRegressor model;
    model.fit(ds);
 
    model.printParameters();
    model.printEquation();

    model.verifyParameters(-153.5129, 1.2390, 12.0813);
 
    std::vector<std::pair<double,double>> experimentos = {
        {45.0, 30.0},
        {55.0, 31.0},
        {60.0, 31.5},
        {70.0, 32.0},
        {80.0, 33.0},
        {50.0, 29.5}
    };
    model.simulate(experimentos);
 
    std::cout << "\n============================================================\n";
    std::cout << "  Programa finalizado correctamente.\n";
    std::cout << "============================================================\n";
    return 0;
}