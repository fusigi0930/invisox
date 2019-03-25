#ifndef CINTERPENVSTORE_WIN_H
#define CINTERPENVSTORE_WIN_H

#include "cenvstore.h"

class CInterpEnvStore : public CEnvStore
{
	Q_OBJECT
public:
	CInterpEnvStore();
	virtual ~CInterpEnvStore();

private:
	bool getInclude();

protected:
	virtual bool parser();

public slots:
	bool slotParser();
};

#endif // CINTERPENVSTORE_WIN_H
