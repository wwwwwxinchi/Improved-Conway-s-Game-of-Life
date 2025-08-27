#include <QApplication>
#include <QWidget>
#include <QPainter>
#include <QTimer>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QVector>
#include <QTime>
#include <cstdlib>

class LifeGame : public QWidget {
    Q_OBJECT
public:
    LifeGame(QWidget *parent = 0) : QWidget(parent) {
        rows = 60;
        cols = 80;
        cellSize = 10;
        running = false;
        showGrid = true;
        speedMs = 100;
        reproduceAbility=4;

        // 初始化网格
        grid.resize(rows * cols);
        grid.fill(0);

        timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(step()));
        setMinimumSize(cols * cellSize, rows * cellSize);

        // 用旧版随机数
        qsrand(static_cast<uint>(QTime::currentTime().msec()));
    }

protected:
    //画cell
    void paintEvent(QPaintEvent *) {
        QPainter p(this);
        p.fillRect(rect(), Qt::white);
        p.setPen(Qt::NoPen);
        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                if (cell(r, c)==1){
                    p.setBrush(Qt::black);
                    p.drawRect(c * cellSize, r * cellSize, cellSize, cellSize);}
                else{
                    if (cell(r, c)==2){
                        p.setBrush(Qt::red);
                        p.drawRect(c * cellSize, r * cellSize, cellSize, cellSize);}
                }
            }
            }
    //如果show网格
        if (showGrid) {
            p.setPen(QColor(220, 220, 220));
            for (int r = 0; r <= rows; ++r)
                p.drawLine(0, r * cellSize, cols * cellSize, r * cellSize);
            for (int c = 0; c <= cols; ++c)
                p.drawLine(c * cellSize, 0, c * cellSize, rows * cellSize);
        }
    }
    //mouse操控
    void mousePressEvent(QMouseEvent *e) {
         // 左，黑点
        if (e->button() == Qt::LeftButton) {
            setCellFromMouse(e->pos(), 1);
            //右，red点
        } else if (e->button() == Qt::RightButton) {
            setCellFromMouse(e->pos(), 2);
        }
    }

    void mouseMoveEvent(QMouseEvent *e) {
        if (e->buttons() & Qt::LeftButton) {
            setCellFromMouse(e->pos(), 1);
        } else if (e->buttons() & Qt::RightButton) {
            setCellFromMouse(e->pos(),2);
        }
    }

    void keyPressEvent(QKeyEvent *e) {
        switch (e->key()) {
        //空格
        case Qt::Key_Space:
            running = !running;
            if (running) timer->start(speedMs);
            else timer->stop();
            break;
          //
        case Qt::Key_S:
            step();
            break;
        case Qt::Key_R:
            randomFill();
            break;
        case Qt::Key_C:
            clear();
            break;
        case Qt::Key_G:
            showGrid = !showGrid;
            update();
            break;
        }
    }

private slots:
    void step() {
        QVector<int> newGrid(rows * cols);
        newGrid.fill(0);
        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                int alive = aliveNeighbors(r, c);
                if(cell(r,c)==2){//2是core,当存在一个1，2 is consistent，如果around cell>4,2 tend to reproduce
                    if(findBlackCell(r,c)>=1){
                        newGrid[r*cols+c]=2;
                        if(findBlackCell(r,c)>=reproduceAbility){
                            reproduce(r,c,newGrid);
                        }
                    }
                    }
                else{
                if (cell(r, c)==1) {
                    if(alive == 2 || alive == 3){
                    newGrid[r * cols + c] =1;}
                } else {
                    newGrid[r * cols + c] = (alive == 3);
                }
            }}
        }
        grid = newGrid;
        update();
    }

private:
    int cell(int r, int c) const {
        return grid[r * cols + c];
    }

    void setCell(int r, int c, int alive) {
        if (r >= 0 && r < rows && c >= 0 && c < cols)
            grid[r * cols + c] = alive;
        update();
    }

    void setCellFromMouse(const QPoint &pos, int alive) {
        int c = pos.x() / cellSize;
        int r = pos.y() / cellSize;
        setCell(r, c, alive);
    }

    int findBlackCell(int r,int c) {
        int count=0;
        for(int dr=-1;dr<=1;dr++){
            for(int dc=-1;dc<=1;dc++){
                if(dr==0&&dc==0)continue;
                int rr = r + dr;
                int cc = c + dc;
                 if (rr >= 0 && rr < rows && cc >= 0 && cc < cols) {
                     if(cell(rr,cc)==1)count++;
                 }
            }
        }
        return count;
    }

    int aliveNeighbors(int r, int c) const {
        int cnt = 0;
        for (int dr = -1; dr <= 1; dr++) {
            for (int dc = -1; dc <= 1; dc++) {
                if (dr == 0 && dc == 0) continue;
                int rr = r + dr;
                int cc = c + dc;
                if (rr >= 0 && rr < rows && cc >= 0 && cc < cols) {
                    if (cell(rr, cc)>0) cnt+=cell(rr, cc);
                }
            }
        }
        return cnt;
    }

    void reproduce(int r,int c, QVector<int> &newgrid){
        for (int dr = -1; dr <= 1; dr++) {
            for (int dc = -1; dc <= 1; dc++) {
                if (dr == 0 && dc == 0) continue;
                int rr = r + dr;
                int cc = c + dc;
                if (rr >= 0 && rr < rows && cc >= 0 && cc < cols) {
                    if (cell(rr, cc)==0) {
                       newgrid[rr * cols + cc]=1;
                        return;}
                }
            }
        }
        return;
    }

    void randomFill() {
        for (int i = 0; i < grid.size(); ++i) {
            int randomValue = qrand() % 100;

            if (randomValue < 5) {// 5% 概率特殊
                grid[i] = 2;
            }
            else if (randomValue < 25) { // 20% 概率黑色普通
                grid[i] = 1;
            }
            else {  // 75% 概率死胞
                grid[i] = 0;
            }
        }
        update();
    }

    void clear() {
        grid.fill(0);
        update();
    }

    int rows, cols, cellSize, speedMs,reproduceAbility;
    bool running, showGrid;
    QVector<int> grid;
    QTimer *timer;
};

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    LifeGame w;
    w.show();
    return a.exec();
}

#include "main.moc"
