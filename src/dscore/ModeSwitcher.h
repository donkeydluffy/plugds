#pragma once

#include <QWidget>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QButtonGroup;
QT_END_NAMESPACE

namespace sss::dscore {

class ModeSwitcher : public QWidget {
  Q_OBJECT

 public:
  explicit ModeSwitcher(QWidget* parent = nullptr);
  ~ModeSwitcher() override;

  void AddModeButton(const QString& id, const QString& title, const QIcon& icon);
  void SetActiveMode(const QString& id);

 signals:
  void ModeSelected(const QString& id);

 private:
  QVBoxLayout* layout_ = nullptr;
  QButtonGroup* button_group_ = nullptr;
};

}  // namespace sss::dscore
