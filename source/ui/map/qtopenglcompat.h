#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
# include <QGLFunctions>
typedef QGLFunctions QOpenGLFunctions;

# include <QGLShaderProgram>
typedef QGLShaderProgram QOpenGLShaderProgram;

# include <QGLShader>
typedef QGLShader QOpenGLShader;

# include <QGLBuffer>
typedef QGLBuffer QOpenGLBuffer;

#else
# include <QOpenGLFunctions>
# include <QOpenGLShaderProgram>
# include <QOpenGLBuffer>
#endif